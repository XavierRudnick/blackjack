#!/usr/bin/env python3
"""
Shared risk-of-ruin helpers for blackjack bet-spread analysis.
"""

from __future__ import annotations

import math
from dataclasses import dataclass
from typing import Iterable


@dataclass(frozen=True)
class RoRResult:
    mu: float
    sigma2: float
    infinite_ror: float
    trip_ror: float
    session_end_ror: float


def normal_cdf(x: float) -> float:
    """Standard normal CDF using the common 5-coefficient approximation."""
    t = 1.0 / (1.0 + 0.2316419 * abs(x))
    d = 0.3989423 * math.exp(-x * x / 2.0)
    p = d * t * (
        0.3193815
        + t * (-0.3565638 + t * (1.781478 + t * (-1.821256 + t * 1.330274)))
    )
    return 1.0 - p if x > 0 else p


def compute_hand_moments(spread, buckets: Iterable) -> tuple[float, float]:
    bucket_list = list(buckets)
    means = []
    second_moments = []

    for bucket in bucket_list:
        bet = spread.bet_for_tc(bucket.tc)
        mean = bet * bucket.wager_multiplier * bucket.ev_per_dollar
        variance = (bet * bucket.wager_multiplier) ** 2 * bucket.variance_per_dollar
        means.append(mean)
        second_moments.append(variance + mean * mean)

    mu = sum(bucket.probability * mean for bucket, mean in zip(bucket_list, means))
    second = sum(
        bucket.probability * moment
        for bucket, moment in zip(bucket_list, second_moments)
    )
    sigma2 = max(0.0, second - mu * mu)
    return mu, sigma2


def infinite_ruin_probability(bankroll_buffer: float, mu: float, sigma2: float) -> float:
    if bankroll_buffer <= 0:
        return 1.0
    if sigma2 <= 1e-12:
        return 1.0 if mu <= 0 else 0.0
    if mu <= 0:
        return 1.0
    return min(1.0, max(0.0, math.exp((-2.0 * mu * bankroll_buffer) / sigma2)))


def finite_trip_ruin_probability(
    bankroll_buffer: float,
    hands: int,
    mu: float,
    sigma2: float,
) -> float:
    if bankroll_buffer <= 0:
        return 1.0
    if hands <= 0:
        return 0.0

    sigma = math.sqrt(sigma2)
    if sigma <= 1e-12:
        return 1.0 if hands * mu <= -bankroll_buffer else 0.0

    horizon = float(hands)
    denom = sigma * math.sqrt(horizon)
    term1 = normal_cdf((-bankroll_buffer - mu * horizon) / denom)
    term2 = infinite_ruin_probability(bankroll_buffer, mu, sigma2) * normal_cdf(
        (-bankroll_buffer + mu * horizon) / denom
    )
    return min(1.0, max(0.0, term1 + term2))


def finite_end_ruin_probability(
    bankroll_buffer: float,
    hands: int,
    mu: float,
    sigma2: float,
) -> float:
    if bankroll_buffer <= 0:
        return 1.0
    if hands <= 0:
        return 0.0

    session_mean = hands * mu
    session_std = math.sqrt(hands * sigma2)
    if session_std <= 1e-12:
        return 1.0 if session_mean <= -bankroll_buffer else 0.0

    return normal_cdf((-bankroll_buffer - session_mean) / session_std)


def analyze_ror(spread, buckets: Iterable, bankroll_buffer: float, hands: int) -> RoRResult:
    mu, sigma2 = compute_hand_moments(spread, buckets)
    return RoRResult(
        mu=mu,
        sigma2=sigma2,
        infinite_ror=infinite_ruin_probability(bankroll_buffer, mu, sigma2),
        trip_ror=finite_trip_ruin_probability(bankroll_buffer, hands, mu, sigma2),
        session_end_ror=finite_end_ruin_probability(bankroll_buffer, hands, mu, sigma2),
    )
