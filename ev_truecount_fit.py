#!/usr/bin/env python3
"""
EV vs True Count Best Fit Analysis for Blackjack Monte Carlo Data

This script reads evPerTC CSV files and fits non-linear models to determine
how true count impacts expected value. Uses weighted regression based on
sample size (HandsPlayed) for more reliable estimates.

Models compared:
- Polynomial regression (degrees 2-5)
- Cubic spline interpolation  
- LOESS/LOWESS smoothing
- Piecewise linear regression

Focuses on 75pen (75% penetration) CSVs by default.
"""

import os
import glob
import argparse
import warnings
from dataclasses import dataclass
from typing import Optional, Tuple, List, Dict

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats
from scipy.interpolate import UnivariateSpline
from scipy.optimize import curve_fit

# Optional imports for advanced models
try:
    from sklearn.preprocessing import PolynomialFeatures
    from sklearn.linear_model import LinearRegression
    from sklearn.metrics import r2_score, mean_squared_error
    HAS_SKLEARN = True
except ImportError:
    HAS_SKLEARN = False
    print("Warning: sklearn not available. Some features disabled.")

try:
    import statsmodels.api as sm
    from statsmodels.nonparametric.smoothers_lowess import lowess
    HAS_STATSMODELS = True
except ImportError:
    HAS_STATSMODELS = False
    print("Warning: statsmodels not available. LOWESS disabled.")


@dataclass
class FitResult:
    """Container for model fit results"""
    model_name: str
    r_squared: float
    adj_r_squared: float
    rmse: float
    aic: float
    bic: float
    coefficients: Optional[np.ndarray] = None
    equation_str: str = ""
    predict_func: Optional[callable] = None


class EVTrueCountAnalyzer:
    """Analyzes relationship between True Count and Expected Value"""
    
    def __init__(self, min_hands: int = 1000, tc_range: Tuple[float, float] = (-15, 20)):
        """
        Args:
            min_hands: Minimum hands played to include a TC data point
            tc_range: (min_tc, max_tc) range to consider for fitting
        """
        self.min_hands = min_hands
        self.tc_range = tc_range
        self.results: Dict[str, List[FitResult]] = {}
        
    def load_csv(self, filepath: str) -> pd.DataFrame:
        """Load and preprocess a single CSV file"""
        df = pd.read_csv(filepath)
        
        # Filter by minimum hands and TC range
        df = df[df['HandsPlayed'] >= self.min_hands]
        df = df[(df['TrueCount'] >= self.tc_range[0]) & 
                (df['TrueCount'] <= self.tc_range[1])]
        
        # Sort by true count
        df = df.sort_values('TrueCount').reset_index(drop=True)
        
        return df
    
    def find_75pen_files(self, base_dir: str) -> List[str]:
        """Find all 75pen CSV files in the evPerTC directory"""
        pattern = os.path.join(base_dir, '**', '*75pen*.csv')
        files = glob.glob(pattern, recursive=True)
        return sorted(files)
    
    def find_files_by_pattern(self, base_dir: str, pattern: str) -> List[str]:
        """Find CSV files matching a pattern"""
        full_pattern = os.path.join(base_dir, '**', f'*{pattern}*.csv')
        files = glob.glob(full_pattern, recursive=True)
        return sorted(files)
    
    def _calculate_aic_bic(self, n: int, k: int, sse: float) -> Tuple[float, float]:
        """Calculate AIC and BIC for model selection"""
        if sse <= 0 or n <= k:
            return float('inf'), float('inf')
        
        # Log-likelihood (assuming normal errors)
        log_likelihood = -n/2 * (np.log(2*np.pi) + np.log(sse/n) + 1)
        
        aic = 2*k - 2*log_likelihood
        bic = k*np.log(n) - 2*log_likelihood
        
        return aic, bic
    
    def _weighted_polynomial_fit(self, tc: np.ndarray, ev: np.ndarray, 
                                  weights: np.ndarray, degree: int) -> FitResult:
        """Fit weighted polynomial regression"""
        # Normalize weights
        w = weights / weights.sum()
        
        # Fit polynomial with weights
        coeffs = np.polyfit(tc, ev, degree, w=np.sqrt(weights))
        poly = np.poly1d(coeffs)
        
        # Predictions
        ev_pred = poly(tc)
        
        # Metrics
        n = len(tc)
        k = degree + 1  # number of parameters
        
        # Weighted residuals
        residuals = ev - ev_pred
        sse = np.sum(weights * residuals**2)
        sst = np.sum(weights * (ev - np.average(ev, weights=weights))**2)
        
        r_squared = 1 - sse/sst if sst > 0 else 0
        adj_r_squared = 1 - (1-r_squared)*(n-1)/(n-k-1) if n > k+1 else r_squared
        rmse = np.sqrt(np.mean(residuals**2))
        
        aic, bic = self._calculate_aic_bic(n, k, sse)
        
        # Build equation string
        terms = []
        for i, c in enumerate(coeffs):
            power = degree - i
            if power == 0:
                terms.append(f"{c:.6f}")
            elif power == 1:
                terms.append(f"{c:.6f}*TC")
            else:
                terms.append(f"{c:.6f}*TC^{power}")
        eq_str = "EV = " + " + ".join(terms).replace("+ -", "- ")
        
        return FitResult(
            model_name=f"Polynomial (degree={degree})",
            r_squared=r_squared,
            adj_r_squared=adj_r_squared,
            rmse=rmse,
            aic=aic,
            bic=bic,
            coefficients=coeffs,
            equation_str=eq_str,
            predict_func=poly
        )
    
    def _cubic_spline_fit(self, tc: np.ndarray, ev: np.ndarray, 
                          weights: np.ndarray, smoothing: float = None) -> FitResult:
        """Fit cubic smoothing spline with moderate smoothing to avoid overfitting"""
        # UnivariateSpline with weights - use higher smoothing to avoid overfitting
        if smoothing is None:
            # Auto-determine smoothing parameter - target ~8-12 knots
            # Higher smoothing = fewer knots = less overfitting
            weighted_var = np.average((ev - np.average(ev, weights=weights))**2, weights=weights)
            smoothing = len(tc) * weighted_var * 50  # Increased multiplier for fewer knots
        
        try:
            spline = UnivariateSpline(tc, ev, w=np.sqrt(weights), s=smoothing, k=3)
            # If still too many knots, increase smoothing
            while len(spline.get_knots()) > 15 and smoothing < len(tc) * 10:
                smoothing *= 2
                spline = UnivariateSpline(tc, ev, w=np.sqrt(weights), s=smoothing, k=3)
        except Exception:
            # Fallback with default smoothing
            spline = UnivariateSpline(tc, ev, s=len(tc)*0.5, k=3)
        
        ev_pred = spline(tc)
        
        n = len(tc)
        # Approximate k as number of knots + 4
        k = len(spline.get_knots()) + 4
        
        residuals = ev - ev_pred
        sse = np.sum(weights * residuals**2)
        sst = np.sum(weights * (ev - np.average(ev, weights=weights))**2)
        
        r_squared = 1 - sse/sst if sst > 0 else 0
        adj_r_squared = 1 - (1-r_squared)*(n-1)/(n-k-1) if n > k+1 else r_squared
        rmse = np.sqrt(np.mean(residuals**2))
        
        aic, bic = self._calculate_aic_bic(n, k, sse)
        
        knots = spline.get_knots()
        eq_str = f"Cubic Spline with {len(knots)} knots at TC={knots.round(1).tolist()}"
        
        return FitResult(
            model_name="Cubic Spline",
            r_squared=r_squared,
            adj_r_squared=adj_r_squared,
            rmse=rmse,
            aic=aic,
            bic=bic,
            equation_str=eq_str,
            predict_func=spline
        )
    
    def _lowess_fit(self, tc: np.ndarray, ev: np.ndarray, 
                    weights: np.ndarray, frac: float = 0.3) -> Optional[FitResult]:
        """Fit LOWESS (Locally Weighted Scatterplot Smoothing)"""
        if not HAS_STATSMODELS:
            return None
        
        # LOWESS doesn't directly support weights, but we can use it
        smoothed = lowess(ev, tc, frac=frac, return_sorted=True)
        
        # Create interpolation function for predictions
        from scipy.interpolate import interp1d
        predict_func = interp1d(smoothed[:, 0], smoothed[:, 1], 
                                kind='linear', fill_value='extrapolate')
        
        ev_pred = predict_func(tc)
        
        n = len(tc)
        k = int(n * frac)  # Approximate effective parameters
        
        residuals = ev - ev_pred
        sse = np.sum(weights * residuals**2)
        sst = np.sum(weights * (ev - np.average(ev, weights=weights))**2)
        
        r_squared = 1 - sse/sst if sst > 0 else 0
        adj_r_squared = 1 - (1-r_squared)*(n-1)/(n-k-1) if n > k+1 else r_squared
        rmse = np.sqrt(np.mean(residuals**2))
        
        aic, bic = self._calculate_aic_bic(n, k, sse)
        
        return FitResult(
            model_name=f"LOWESS (frac={frac})",
            r_squared=r_squared,
            adj_r_squared=adj_r_squared,
            rmse=rmse,
            aic=aic,
            bic=bic,
            equation_str=f"LOWESS with fraction={frac}",
            predict_func=predict_func
        )
    
    def _piecewise_linear_fit(self, tc: np.ndarray, ev: np.ndarray,
                               weights: np.ndarray, breakpoint: float = 0) -> FitResult:
        """Fit piecewise linear model with a breakpoint"""
        
        def piecewise_linear(x, a1, b1, a2, b2, x0):
            """Piecewise linear function with continuity at breakpoint"""
            return np.where(x < x0, 
                           a1 * x + b1, 
                           a2 * (x - x0) + (a1 * x0 + b1))
        
        # Initial guess
        p0 = [0.005, -0.01, 0.005, 0, breakpoint]
        
        try:
            # Fit with weights via sigma
            sigma = 1 / np.sqrt(weights + 1)
            popt, pcov = curve_fit(piecewise_linear, tc, ev, p0=p0, 
                                   sigma=sigma, maxfev=10000,
                                   bounds=([-0.1, -0.5, -0.1, -0.5, -5],
                                          [0.1, 0.5, 0.1, 0.5, 5]))
            
            a1, b1, a2, b2, x0 = popt
            ev_pred = piecewise_linear(tc, *popt)
            
            n = len(tc)
            k = 5  # 5 parameters
            
            residuals = ev - ev_pred
            sse = np.sum(weights * residuals**2)
            sst = np.sum(weights * (ev - np.average(ev, weights=weights))**2)
            
            r_squared = 1 - sse/sst if sst > 0 else 0
            adj_r_squared = 1 - (1-r_squared)*(n-1)/(n-k-1) if n > k+1 else r_squared
            rmse = np.sqrt(np.mean(residuals**2))
            
            aic, bic = self._calculate_aic_bic(n, k, sse)
            
            eq_str = (f"EV = {a1:.6f}*TC + {b1:.6f} for TC < {x0:.2f}\n"
                     f"EV = {a2:.6f}*(TC - {x0:.2f}) + {a1*x0+b1:.6f} for TC >= {x0:.2f}")
            
            predict_func = lambda x: piecewise_linear(x, *popt)
            
            return FitResult(
                model_name="Piecewise Linear",
                r_squared=r_squared,
                adj_r_squared=adj_r_squared,
                rmse=rmse,
                aic=aic,
                bic=bic,
                coefficients=popt,
                equation_str=eq_str,
                predict_func=predict_func
            )
        except Exception as e:
            warnings.warn(f"Piecewise linear fit failed: {e}")
            return None
    
    def _weighted_linear_fit(self, tc: np.ndarray, ev: np.ndarray,
                              weights: np.ndarray) -> FitResult:
        """Fit simple weighted linear regression as baseline"""
        # Weighted linear regression
        w = weights / weights.sum()
        
        # Fit linear (degree 1)
        coeffs = np.polyfit(tc, ev, 1, w=np.sqrt(weights))
        slope, intercept = coeffs
        poly = np.poly1d(coeffs)
        
        ev_pred = poly(tc)
        
        n = len(tc)
        k = 2  # slope + intercept
        
        residuals = ev - ev_pred
        sse = np.sum(weights * residuals**2)
        sst = np.sum(weights * (ev - np.average(ev, weights=weights))**2)
        
        r_squared = 1 - sse/sst if sst > 0 else 0
        adj_r_squared = 1 - (1-r_squared)*(n-1)/(n-k-1) if n > k+1 else r_squared
        rmse = np.sqrt(np.mean(residuals**2))
        
        aic, bic = self._calculate_aic_bic(n, k, sse)
        
        # Convert slope to percentage per TC
        slope_pct = slope * 100
        intercept_pct = intercept * 100
        
        eq_str = f"EV = {slope:.6f}*TC + {intercept:.6f}  (≈ {slope_pct:.3f}% per TC, base EV = {intercept_pct:.3f}%)"
        
        return FitResult(
            model_name="Linear (baseline)",
            r_squared=r_squared,
            adj_r_squared=adj_r_squared,
            rmse=rmse,
            aic=aic,
            bic=bic,
            coefficients=coeffs,
            equation_str=eq_str,
            predict_func=poly
        )

    def fit_all_models(self, df: pd.DataFrame) -> List[FitResult]:
        """Fit all model types to the data"""
        tc = df['TrueCount'].values
        ev = df['EV'].values
        weights = df['HandsPlayed'].values.astype(float)
        
        results = []
        
        # Linear baseline first
        try:
            result = self._weighted_linear_fit(tc, ev, weights)
            results.append(result)
        except Exception as e:
            warnings.warn(f"Linear fit failed: {e}")
        
        # Polynomial fits (degrees 2-5)
        for degree in [2, 3, 4, 5]:
            try:
                result = self._weighted_polynomial_fit(tc, ev, weights, degree)
                results.append(result)
            except Exception as e:
                warnings.warn(f"Polynomial degree {degree} fit failed: {e}")
        
        # Cubic spline
        try:
            result = self._cubic_spline_fit(tc, ev, weights)
            results.append(result)
        except Exception as e:
            warnings.warn(f"Cubic spline fit failed: {e}")
        
        # LOWESS
        for frac in [0.2, 0.3, 0.4]:
            result = self._lowess_fit(tc, ev, weights, frac)
            if result:
                results.append(result)
        
        # Piecewise linear
        result = self._piecewise_linear_fit(tc, ev, weights)
        if result:
            results.append(result)
        
        # Sort by AIC (lower is better)
        results.sort(key=lambda x: x.aic)
        
        return results
    
    def analyze_file(self, filepath: str, plot: bool = True, 
                     save_dir: Optional[str] = None) -> List[FitResult]:
        """Analyze a single CSV file"""
        print(f"\n{'='*70}")
        print(f"Analyzing: {os.path.basename(filepath)}")
        print('='*70)
        
        df = self.load_csv(filepath)
        
        if len(df) < 10:
            print(f"  Insufficient data points ({len(df)}). Skipping.")
            return []
        
        print(f"  Data points: {len(df)} (TC range: {df['TrueCount'].min():.1f} to {df['TrueCount'].max():.1f})")
        print(f"  Total hands: {df['HandsPlayed'].sum():,.0f}")
        
        results = self.fit_all_models(df)
        
        # Print results table
        print("\n  Model Comparison (sorted by AIC):")
        print("  " + "-"*66)
        print(f"  {'Model':<25} {'R²':>8} {'Adj R²':>8} {'RMSE':>10} {'AIC':>12}")
        print("  " + "-"*66)
        
        for r in results:
            print(f"  {r.model_name:<25} {r.r_squared:>8.4f} {r.adj_r_squared:>8.4f} "
                  f"{r.rmse:>10.6f} {r.aic:>12.1f}")
        
        # Best model details
        best = results[0]
        print(f"\n  Best Model (by AIC): {best.model_name}")
        print(f"  Equation: {best.equation_str}")
        
        # Find best polynomial for interpretability
        poly_results = [r for r in results if r.model_name.startswith('Polynomial')]
        if poly_results:
            best_poly = min(poly_results, key=lambda x: x.aic)
            print(f"\n  Best Polynomial: {best_poly.model_name}")
            print(f"  Equation: {best_poly.equation_str}")
        
        # Show linear baseline for reference
        linear = [r for r in results if r.model_name == 'Linear (baseline)']
        if linear:
            lin = linear[0]
            print(f"\n  Linear Baseline: {lin.equation_str}")
            print(f"  Linear R²: {lin.r_squared:.4f}")
        
        if plot:
            self._plot_results(df, results[:4], filepath, save_dir)
        
        return results
    
    def _plot_results(self, df: pd.DataFrame, results: List[FitResult], 
                      filepath: str, save_dir: Optional[str] = None):
        """Create visualization of fit results"""
        tc = df['TrueCount'].values
        ev = df['EV'].values
        weights = df['HandsPlayed'].values
        
        # Normalize marker sizes
        sizes = 20 + 80 * (weights - weights.min()) / (weights.max() - weights.min() + 1)
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        axes = axes.flatten()
        
        # Create smooth x values for plotting
        tc_smooth = np.linspace(tc.min(), tc.max(), 200)
        
        colors = ['#e74c3c', '#3498db', '#2ecc71', '#9b59b6']
        
        for idx, (ax, result) in enumerate(zip(axes, results)):
            # Scatter plot with size by weight
            ax.scatter(tc, ev * 100, s=sizes, alpha=0.6, c='gray', 
                      edgecolors='black', linewidth=0.5, label='Data')
            
            # Plot fit line
            if result.predict_func is not None:
                try:
                    ev_smooth = result.predict_func(tc_smooth)
                    ax.plot(tc_smooth, ev_smooth * 100, color=colors[idx], 
                           linewidth=2.5, label=result.model_name)
                except Exception:
                    pass
            
            ax.axhline(y=0, color='black', linestyle='--', alpha=0.3)
            ax.axvline(x=0, color='black', linestyle='--', alpha=0.3)
            
            ax.set_xlabel('True Count', fontsize=10)
            ax.set_ylabel('EV (%)', fontsize=10)
            ax.set_title(f"{result.model_name}\nR²={result.r_squared:.4f}, AIC={result.aic:.1f}", 
                        fontsize=11)
            ax.legend(loc='upper left', fontsize=9)
            ax.grid(True, alpha=0.3)
        
        # Extract filename for title
        filename = os.path.basename(filepath).replace('.csv', '')
        fig.suptitle(f'EV vs True Count Analysis\n{filename}', fontsize=13, y=1.02)
        
        plt.tight_layout()
        
        if save_dir:
            os.makedirs(save_dir, exist_ok=True)
            save_path = os.path.join(save_dir, filename + '_fit.png')
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to: {save_path}")
        
        plt.show()
    
    def analyze_all_75pen(self, base_dir: str, save_dir: Optional[str] = None,
                          plot: bool = True) -> Dict[str, List[FitResult]]:
        """Analyze all 75pen files in directory"""
        files = self.find_75pen_files(base_dir)
        
        if not files:
            print(f"No 75pen files found in {base_dir}")
            return {}
        
        print(f"Found {len(files)} 75pen files")
        
        all_results = {}
        for filepath in files:
            key = os.path.basename(filepath)
            try:
                results = self.analyze_file(filepath, plot=plot, save_dir=save_dir)
                if results:
                    all_results[key] = results
            except Exception as e:
                print(f"  Error processing {key}: {e}")
        
        return all_results
    
    def generate_summary_report(self, all_results: Dict[str, List[FitResult]], 
                                 output_path: str = 'ev_tc_fit_summary.csv'):
        """Generate summary CSV of best fits for all files"""
        rows = []
        for filename, results in all_results.items():
            if not results:
                continue
            best = results[0]
            
            # Parse filename for metadata
            parts = filename.replace('.csv', '').split('_')
            strategy = parts[3] if len(parts) > 3 else 'Unknown'
            deck_info = parts[4] if len(parts) > 4 else 'Unknown'
            
            rows.append({
                'Filename': filename,
                'Strategy': strategy,
                'DeckConfig': deck_info,
                'BestModel': best.model_name,
                'R_Squared': best.r_squared,
                'Adj_R_Squared': best.adj_r_squared,
                'RMSE': best.rmse,
                'AIC': best.aic,
                'BIC': best.bic,
                'Equation': best.equation_str
            })
        
        df = pd.DataFrame(rows)
        df.to_csv(output_path, index=False)
        print(f"\nSummary report saved to: {output_path}")
        return df


def main():
    parser = argparse.ArgumentParser(
        description='Analyze EV vs True Count relationship with non-linear fits'
    )
    parser.add_argument(
        '--dir', '-d',
        default='stats_H17_150mil/evPerTC',
        help='Base directory containing strategy subdirectories with CSV files'
    )
    parser.add_argument(
        '--pattern', '-p',
        default='75pen',
        help='Pattern to match in filenames (default: 75pen)'
    )
    parser.add_argument(
        '--file', '-f',
        help='Analyze a single specific CSV file'
    )
    parser.add_argument(
        '--min-hands', '-m',
        type=int,
        default=1000,
        help='Minimum hands played to include data point (default: 1000)'
    )
    parser.add_argument(
        '--tc-min',
        type=float,
        default=-15,
        help='Minimum true count to consider (default: -15)'
    )
    parser.add_argument(
        '--tc-max',
        type=float,
        default=20,
        help='Maximum true count to consider (default: 20)'
    )
    parser.add_argument(
        '--no-plot',
        action='store_true',
        help='Disable plotting'
    )
    parser.add_argument(
        '--save-plots',
        help='Directory to save plots'
    )
    parser.add_argument(
        '--output', '-o',
        default='ev_tc_fit_summary.csv',
        help='Output CSV for summary report'
    )
    
    args = parser.parse_args()
    
    analyzer = EVTrueCountAnalyzer(
        min_hands=args.min_hands,
        tc_range=(args.tc_min, args.tc_max)
    )
    
    if args.file:
        # Analyze single file
        results = analyzer.analyze_file(
            args.file, 
            plot=not args.no_plot,
            save_dir=args.save_plots
        )
    else:
        # Analyze all matching files
        base_dir = args.dir
        
        if args.pattern == '75pen':
            all_results = analyzer.analyze_all_75pen(
                base_dir, 
                save_dir=args.save_plots,
                plot=not args.no_plot
            )
        else:
            files = analyzer.find_files_by_pattern(base_dir, args.pattern)
            all_results = {}
            for f in files:
                key = os.path.basename(f)
                try:
                    results = analyzer.analyze_file(f, plot=not args.no_plot, 
                                                   save_dir=args.save_plots)
                    if results:
                        all_results[key] = results
                except Exception as e:
                    print(f"Error: {e}")
        
        if all_results:
            analyzer.generate_summary_report(all_results, args.output)


if __name__ == '__main__':
    main()
