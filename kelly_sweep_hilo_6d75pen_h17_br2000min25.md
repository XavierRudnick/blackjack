Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  90 TC buckets in [-23.0 .. +23.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 43,061,863 hands (avg 43.1 hands/shoe) in 9.3s

→ Kelly k=0.250   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,647 replays/s
     6,000/30,000  ( 20.0%)  2,638 replays/s
     9,000/30,000  ( 30.0%)  2,636 replays/s
    12,000/30,000  ( 40.0%)  2,634 replays/s
    15,000/30,000  ( 50.0%)  2,633 replays/s
    18,000/30,000  ( 60.0%)  2,635 replays/s
    21,000/30,000  ( 70.0%)  2,637 replays/s
    24,000/30,000  ( 80.0%)  2,636 replays/s
    27,000/30,000  ( 90.0%)  2,636 replays/s
    30,000/30,000  (100.0%)  2,633 replays/s
  done in 11.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.250 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $      225
Median bucket bet              $                 25
Spread (max / min)                            9.00×
Buckets at table min                             85
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 25
TC +6                          $                 25
TC +7                          $                 25
TC +8                          $                 25
TC +9                          $                 25
TC +10                         $                 25

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            -124.59
P(finish down)  (end < B0)                   55.43%
P(ruin)                                       2.65%
P(end < 0.9·B0)                              46.17%
P(end < 0.75·B0)                             32.76%
P(end < 0.5·B0)                              15.01%
P(end < 0.25·B0)                              5.48%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,875.41
Median ending bankroll         $           1,875.00
95% range [p2.5 .. p97.5]      $    12.50  ..  $ 3,537.50
P(lose > 25%)                                32.76%
P(lose > 50%)                                15.01%
P(double bankroll)                            0.68%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             436.96
Median max drawdown            $             425.00
95th pct max drawdown          $             587.50
99th pct max drawdown          $             662.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.4908%
Avg session wagered            $          24,467.11

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.9
Avg hands played                              856.7
======================================================================

→ Kelly k=0.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,647 replays/s
     6,000/30,000  ( 20.0%)  2,652 replays/s
     9,000/30,000  ( 30.0%)  2,651 replays/s
    12,000/30,000  ( 40.0%)  2,654 replays/s
    15,000/30,000  ( 50.0%)  2,654 replays/s
    18,000/30,000  ( 60.0%)  2,652 replays/s
    21,000/30,000  ( 70.0%)  2,653 replays/s
    24,000/30,000  ( 80.0%)  2,651 replays/s
    27,000/30,000  ( 90.0%)  2,650 replays/s
    30,000/30,000  (100.0%)  2,648 replays/s
  done in 11.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.500 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $      450
Median bucket bet              $                 25
Spread (max / min)                           18.00×
Buckets at table min                             70
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 25
TC +6                          $                 25
TC +7                          $                 25
TC +8                          $                 25
TC +9                          $                 50
TC +10                         $                 50

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            -121.11
P(finish down)  (end < B0)                   55.29%
P(ruin)                                       2.67%
P(end < 0.9·B0)                              45.97%
P(end < 0.75·B0)                             32.78%
P(end < 0.5·B0)                              15.01%
P(end < 0.25·B0)                              5.51%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,878.89
Median ending bankroll         $           1,887.50
95% range [p2.5 .. p97.5]      $    12.50  ..  $ 3,562.50
P(lose > 25%)                                32.78%
P(lose > 50%)                                15.01%
P(double bankroll)                            0.70%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             439.37
Median max drawdown            $             425.00
95th pct max drawdown          $             587.50
99th pct max drawdown          $             675.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.5065%
Avg session wagered            $          24,542.53

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.9
Avg hands played                              856.6
======================================================================

→ Kelly k=0.750   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,621 replays/s
     6,000/30,000  ( 20.0%)  2,617 replays/s
     9,000/30,000  ( 30.0%)  2,618 replays/s
    12,000/30,000  ( 40.0%)  2,617 replays/s
    15,000/30,000  ( 50.0%)  2,617 replays/s
    18,000/30,000  ( 60.0%)  2,617 replays/s
    21,000/30,000  ( 70.0%)  2,623 replays/s
    24,000/30,000  ( 80.0%)  2,627 replays/s
    27,000/30,000  ( 90.0%)  2,630 replays/s
    30,000/30,000  (100.0%)  2,634 replays/s
  done in 11.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.750 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $      675
Median bucket bet              $                 25
Spread (max / min)                           27.00×
Buckets at table min                             63
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 25
TC +6                          $                 50
TC +7                          $                 50
TC +8                          $                 50
TC +9                          $                 50
TC +10                         $                 75

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            -109.63
P(finish down)  (end < B0)                   54.47%
P(ruin)                                       3.00%
P(end < 0.9·B0)                              45.46%
P(end < 0.75·B0)                             32.66%
P(end < 0.5·B0)                              15.24%
P(end < 0.25·B0)                              5.79%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,890.37
Median ending bankroll         $           1,900.00
95% range [p2.5 .. p97.5]      $    12.50  ..  $ 3,600.00
P(lose > 25%)                                32.66%
P(lose > 50%)                                15.24%
P(double bankroll)                            0.85%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             453.05
Median max drawdown            $             437.50
95th pct max drawdown          $             625.00
99th pct max drawdown          $             725.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.5594%
Avg session wagered            $          24,881.29

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.9
Avg hands played                              855.7
======================================================================

→ Kelly k=1.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,665 replays/s
     6,000/30,000  ( 20.0%)  2,664 replays/s
     9,000/30,000  ( 30.0%)  2,664 replays/s
    12,000/30,000  ( 40.0%)  2,663 replays/s
    15,000/30,000  ( 50.0%)  2,664 replays/s
    18,000/30,000  ( 60.0%)  2,663 replays/s
    21,000/30,000  ( 70.0%)  2,660 replays/s
    24,000/30,000  ( 80.0%)  2,659 replays/s
    27,000/30,000  ( 90.0%)  2,659 replays/s
    30,000/30,000  (100.0%)  2,658 replays/s
  done in 11.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $      900
Median bucket bet              $                 25
Spread (max / min)                           36.00×
Buckets at table min                             61
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 50
TC +6                          $                 50
TC +7                          $                 50
TC +8                          $                 75
TC +9                          $                 75
TC +10                         $                 75

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -95.54
P(finish down)  (end < B0)                   53.69%
P(ruin)                                       3.37%
P(end < 0.9·B0)                              45.06%
P(end < 0.75·B0)                             32.46%
P(end < 0.5·B0)                              15.68%
P(end < 0.25·B0)                              6.10%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,904.46
Median ending bankroll         $           1,900.00
95% range [p2.5 .. p97.5]      $    12.50  ..  $ 3,662.50
P(lose > 25%)                                32.46%
P(lose > 50%)                                15.68%
P(double bankroll)                            1.06%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             475.07
Median max drawdown            $             450.00
95th pct max drawdown          $             675.00
99th pct max drawdown          $             850.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.6229%
Avg session wagered            $          25,334.66

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.9
Avg hands played                              854.8
======================================================================

→ Kelly k=1.250   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,663 replays/s
     6,000/30,000  ( 20.0%)  2,653 replays/s
     9,000/30,000  ( 30.0%)  2,652 replays/s
    12,000/30,000  ( 40.0%)  2,655 replays/s
    15,000/30,000  ( 50.0%)  2,657 replays/s
    18,000/30,000  ( 60.0%)  2,654 replays/s
    21,000/30,000  ( 70.0%)  2,652 replays/s
    24,000/30,000  ( 80.0%)  2,652 replays/s
    27,000/30,000  ( 90.0%)  2,652 replays/s
    30,000/30,000  (100.0%)  2,653 replays/s
  done in 11.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.250
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.250 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    1,125
Median bucket bet              $                 25
Spread (max / min)                           45.00×
Buckets at table min                             60
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 50
TC +6                          $                 75
TC +7                          $                 75
TC +8                          $                100
TC +9                          $                100
TC +10                         $                100

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -80.64
P(finish down)  (end < B0)                   52.86%
P(ruin)                                       4.12%
P(end < 0.9·B0)                              44.58%
P(end < 0.75·B0)                             32.63%
P(end < 0.5·B0)                              16.28%
P(end < 0.25·B0)                              6.96%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,919.36
Median ending bankroll         $           1,925.00
95% range [p2.5 .. p97.5]      $     0.00  ..  $ 3,762.81
P(lose > 25%)                                32.63%
P(lose > 50%)                                16.28%
P(double bankroll)                            1.46%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             511.30
Median max drawdown            $             475.00
95th pct max drawdown          $             787.50
99th pct max drawdown          $           1,012.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.6884%
Avg session wagered            $          25,880.45

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.8
Avg hands played                              852.8
======================================================================

→ Kelly k=1.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,674 replays/s
     6,000/30,000  ( 20.0%)  2,667 replays/s
     9,000/30,000  ( 30.0%)  2,662 replays/s
    12,000/30,000  ( 40.0%)  2,662 replays/s
    15,000/30,000  ( 50.0%)  2,665 replays/s
    18,000/30,000  ( 60.0%)  2,666 replays/s
    21,000/30,000  ( 70.0%)  2,667 replays/s
    24,000/30,000  ( 80.0%)  2,668 replays/s
    27,000/30,000  ( 90.0%)  2,665 replays/s
    30,000/30,000  (100.0%)  2,666 replays/s
  done in 11.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.500 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    1,350
Median bucket bet              $                 25
Spread (max / min)                           54.00×
Buckets at table min                             59
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 50
TC +5                          $                 50
TC +6                          $                 75
TC +7                          $                100
TC +8                          $                100
TC +9                          $                125
TC +10                         $                125

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -65.45
P(finish down)  (end < B0)                   52.22%
P(ruin)                                       5.01%
P(end < 0.9·B0)                              44.26%
P(end < 0.75·B0)                             32.79%
P(end < 0.5·B0)                              17.05%
P(end < 0.25·B0)                              7.87%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,934.55
Median ending bankroll         $           1,937.50
95% range [p2.5 .. p97.5]      $     0.00  ..  $ 3,875.31
P(lose > 25%)                                32.79%
P(lose > 50%)                                17.05%
P(double bankroll)                            1.89%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             550.94
Median max drawdown            $             512.50
95th pct max drawdown          $             887.50
99th pct max drawdown          $           1,150.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.7527%
Avg session wagered            $          26,467.43

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.8
Avg hands played                              850.4
======================================================================

→ Kelly k=2.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,713 replays/s
     6,000/30,000  ( 20.0%)  2,710 replays/s
     9,000/30,000  ( 30.0%)  2,709 replays/s
    12,000/30,000  ( 40.0%)  2,706 replays/s
    15,000/30,000  ( 50.0%)  2,706 replays/s
    18,000/30,000  ( 60.0%)  2,707 replays/s
    21,000/30,000  ( 70.0%)  2,705 replays/s
    24,000/30,000  ( 80.0%)  2,703 replays/s
    27,000/30,000  ( 90.0%)  2,704 replays/s
    30,000/30,000  (100.0%)  2,702 replays/s
  done in 11.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    1,800
Median bucket bet              $                 25
Spread (max / min)                           72.00×
Buckets at table min                             57
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 50
TC +4                          $                 50
TC +5                          $                 75
TC +6                          $                100
TC +7                          $                125
TC +8                          $                150
TC +9                          $                150
TC +10                         $                175

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -25.96
P(finish down)  (end < B0)                   50.44%
P(ruin)                                       8.13%
P(end < 0.9·B0)                              43.45%
P(end < 0.75·B0)                             33.54%
P(end < 0.5·B0)                              19.63%
P(end < 0.25·B0)                             11.11%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,974.04
Median ending bankroll         $           1,975.00
95% range [p2.5 .. p97.5]      $   -12.50  ..  $ 4,225.00
P(lose > 25%)                                33.54%
P(lose > 50%)                                19.63%
P(double bankroll)                            3.78%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             677.91
Median max drawdown            $             625.00
95th pct max drawdown          $           1,175.00
99th pct max drawdown          $           1,512.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.9082%
Avg session wagered            $          28,270.18

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.5
Avg hands played                              839.4
======================================================================

→ Kelly k=2.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,755 replays/s
     6,000/30,000  ( 20.0%)  2,754 replays/s
     9,000/30,000  ( 30.0%)  2,752 replays/s
    12,000/30,000  ( 40.0%)  2,754 replays/s
    15,000/30,000  ( 50.0%)  2,758 replays/s
    18,000/30,000  ( 60.0%)  2,761 replays/s
    21,000/30,000  ( 70.0%)  2,761 replays/s
    24,000/30,000  ( 80.0%)  2,761 replays/s
    27,000/30,000  ( 90.0%)  2,762 replays/s
    30,000/30,000  (100.0%)  2,763 replays/s
  done in 10.9s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.500 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             56
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 50
TC +4                          $                 75
TC +5                          $                100
TC +6                          $                125
TC +7                          $                150
TC +8                          $                175
TC +9                          $                200
TC +10                         $                225

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $              +8.51
P(finish down)  (end < B0)                   49.39%
P(ruin)                                      12.00%
P(end < 0.9·B0)                              43.26%
P(end < 0.75·B0)                             34.64%
P(end < 0.5·B0)                              22.46%
P(end < 0.25·B0)                             14.70%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,008.51
Median ending bankroll         $           2,012.50
95% range [p2.5 .. p97.5]      $   -62.50  ..  $ 4,575.00
P(lose > 25%)                                34.64%
P(lose > 50%)                                22.46%
P(double bankroll)                            6.37%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             808.83
Median max drawdown            $             737.50
95th pct max drawdown          $           1,437.50
99th pct max drawdown          $           1,837.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.0285%
Avg session wagered            $          29,891.56

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.2
Avg hands played                              825.2
======================================================================

→ Kelly k=3.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,802 replays/s
     6,000/30,000  ( 20.0%)  2,801 replays/s
     9,000/30,000  ( 30.0%)  2,790 replays/s
    12,000/30,000  ( 40.0%)  2,793 replays/s
    15,000/30,000  ( 50.0%)  2,799 replays/s
    18,000/30,000  ( 60.0%)  2,799 replays/s
    21,000/30,000  ( 70.0%)  2,797 replays/s
    24,000/30,000  ( 80.0%)  2,797 replays/s
    27,000/30,000  ( 90.0%)  2,798 replays/s
    30,000/30,000  (100.0%)  2,799 replays/s
  done in 10.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=3.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             56
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 50
TC +4                          $                100
TC +5                          $                125
TC +6                          $                150
TC +7                          $                175
TC +8                          $                200
TC +9                          $                225
TC +10                         $                250

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +34.40
P(finish down)  (end < B0)                   48.89%
P(ruin)                                      14.96%
P(end < 0.9·B0)                              43.25%
P(end < 0.75·B0)                             35.71%
P(end < 0.5·B0)                              24.63%
P(end < 0.25·B0)                             17.45%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,034.40
Median ending bankroll         $           2,025.00
95% range [p2.5 .. p97.5]      $   -87.50  ..  $ 4,887.50
P(lose > 25%)                                35.71%
P(lose > 50%)                                24.63%
P(double bankroll)                            8.45%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             914.14
Median max drawdown            $             837.50
95th pct max drawdown          $           1,637.50
99th pct max drawdown          $           2,087.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.1118%
Avg session wagered            $          30,764.75

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               18.9
Avg hands played                              811.9
======================================================================

→ Kelly k=4.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,950 replays/s
     6,000/30,000  ( 20.0%)  2,937 replays/s
     9,000/30,000  ( 30.0%)  2,938 replays/s
    12,000/30,000  ( 40.0%)  2,937 replays/s
    15,000/30,000  ( 50.0%)  2,939 replays/s
    18,000/30,000  ( 60.0%)  2,939 replays/s
    21,000/30,000  ( 70.0%)  2,937 replays/s
    24,000/30,000  ( 80.0%)  2,937 replays/s
    27,000/30,000  ( 90.0%)  2,937 replays/s
    30,000/30,000  (100.0%)  2,937 replays/s
  done in 10.2s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=4.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             55
Buckets at table max                              3

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                 75
TC +4                          $                125
TC +5                          $                150
TC +6                          $                200
TC +7                          $                250
TC +8                          $                275
TC +9                          $                325
TC +10                         $                350

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +96.76
P(finish down)  (end < B0)                   48.51%
P(ruin)                                      23.57%
P(end < 0.9·B0)                              44.39%
P(end < 0.75·B0)                             38.75%
P(end < 0.5·B0)                              30.81%
P(end < 0.25·B0)                             25.51%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,096.76
Median ending bankroll         $           2,062.50
95% range [p2.5 .. p97.5]      $  -187.50  ..  $ 5,775.00
P(lose > 25%)                                38.75%
P(lose > 50%)                                30.81%
P(double bankroll)                           14.27%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,185.30
Median max drawdown            $           1,100.00
95th pct max drawdown          $           2,100.00
99th pct max drawdown          $           2,637.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.2864%
Avg session wagered            $          33,787.77

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               18.0
Avg hands played                              770.6
======================================================================

→ Kelly k=5.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  3,070 replays/s
     6,000/30,000  ( 20.0%)  3,059 replays/s
     9,000/30,000  ( 30.0%)  3,065 replays/s
    12,000/30,000  ( 40.0%)  3,060 replays/s
    15,000/30,000  ( 50.0%)  3,064 replays/s
    18,000/30,000  ( 60.0%)  3,068 replays/s
    21,000/30,000  ( 70.0%)  3,067 replays/s
    24,000/30,000  ( 80.0%)  3,072 replays/s
    27,000/30,000  ( 90.0%)  3,071 replays/s
    30,000/30,000  (100.0%)  3,071 replays/s
  done in 9.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 5.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=5.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             55
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                100
TC +4                          $                150
TC +5                          $                200
TC +6                          $                250
TC +7                          $                300
TC +8                          $                350
TC +9                          $                400
TC +10                         $                425

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +140.81
P(finish down)  (end < B0)                   49.50%
P(ruin)                                      30.04%
P(end < 0.9·B0)                              46.25%
P(end < 0.75·B0)                             41.78%
P(end < 0.5·B0)                              35.76%
P(end < 0.25·B0)                             31.67%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,140.81
Median ending bankroll         $           2,025.00
95% range [p2.5 .. p97.5]      $  -275.00  ..  $ 6,537.50
P(lose > 25%)                                41.78%
P(lose > 50%)                                35.76%
P(double bankroll)                           18.64%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,409.39
Median max drawdown            $           1,312.50
95th pct max drawdown          $           2,475.00
99th pct max drawdown          $           3,125.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.4033%
Avg session wagered            $          34,918.20

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               17.2
Avg hands played                              735.2
======================================================================

→ Kelly k=6.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  3,218 replays/s
     6,000/30,000  ( 20.0%)  3,220 replays/s
     9,000/30,000  ( 30.0%)  3,229 replays/s
    12,000/30,000  ( 40.0%)  3,222 replays/s
    15,000/30,000  ( 50.0%)  3,227 replays/s
    18,000/30,000  ( 60.0%)  3,231 replays/s
    21,000/30,000  ( 70.0%)  3,227 replays/s
    24,000/30,000  ( 80.0%)  3,232 replays/s
    27,000/30,000  ( 90.0%)  3,230 replays/s
    30,000/30,000  (100.0%)  3,231 replays/s
  done in 9.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 6.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=6.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             55
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                125
TC +4                          $                175
TC +5                          $                225
TC +6                          $                300
TC +7                          $                350
TC +8                          $                425
TC +9                          $                475
TC +10                         $                525

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +182.21
P(finish down)  (end < B0)                   51.10%
P(ruin)                                      36.11%
P(end < 0.9·B0)                              48.49%
P(end < 0.75·B0)                             45.06%
P(end < 0.5·B0)                              40.34%
P(end < 0.25·B0)                             37.38%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,182.21
Median ending bankroll         $           1,912.50
95% range [p2.5 .. p97.5]      $  -350.00  ..  $ 7,375.00
P(lose > 25%)                                45.06%
P(lose > 50%)                                40.34%
P(double bankroll)                           22.08%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,625.42
Median max drawdown            $           1,525.00
95th pct max drawdown          $           2,825.00
99th pct max drawdown          $           3,587.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.5039%
Avg session wagered            $          36,157.93

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               16.3
Avg hands played                              698.5
======================================================================

→ Kelly k=7.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  3,426 replays/s
     6,000/30,000  ( 20.0%)  3,414 replays/s
     9,000/30,000  ( 30.0%)  3,421 replays/s
    12,000/30,000  ( 40.0%)  3,409 replays/s
    15,000/30,000  ( 50.0%)  3,416 replays/s
    18,000/30,000  ( 60.0%)  3,422 replays/s
    21,000/30,000  ( 70.0%)  3,421 replays/s
    24,000/30,000  ( 80.0%)  3,426 replays/s
    27,000/30,000  ( 90.0%)  3,423 replays/s
    30,000/30,000  (100.0%)  3,422 replays/s
  done in 8.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 7.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=7.000 | B0=$2,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 75
TC +3                          $                150
TC +4                          $                200
TC +5                          $                275
TC +6                          $                350
TC +7                          $                425
TC +8                          $                500
TC +9                          $                550
TC +10                         $                600

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +227.00
P(finish down)  (end < B0)                   53.39%
P(ruin)                                      42.02%
P(end < 0.9·B0)                              51.42%
P(end < 0.75·B0)                             48.81%
P(end < 0.5·B0)                              45.25%
P(end < 0.25·B0)                             42.93%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,227.00
Median ending bankroll         $           1,625.00
95% range [p2.5 .. p97.5]      $  -450.00  ..  $ 8,312.81
P(lose > 25%)                                48.81%
P(lose > 50%)                                45.25%
P(double bankroll)                           25.01%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,859.43
Median max drawdown            $           1,750.00
95th pct max drawdown          $           3,200.00
99th pct max drawdown          $           4,100.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.5897%
Avg session wagered            $          38,491.87

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.4
Avg hands played                              658.6
======================================================================

Figure saved to: kelly_sweep_hilo_6d75pen_h17_br2000min25.png
