Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  90 TC buckets in [-23.0 .. +23.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 43,061,863 hands (avg 43.1 hands/shoe) in 9.2s

→ Kelly k=0.250   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,601 replays/s
     6,000/30,000  ( 20.0%)  2,604 replays/s
     9,000/30,000  ( 30.0%)  2,610 replays/s
    12,000/30,000  ( 40.0%)  2,614 replays/s
    15,000/30,000  ( 50.0%)  2,616 replays/s
    18,000/30,000  ( 60.0%)  2,615 replays/s
    21,000/30,000  ( 70.0%)  2,615 replays/s
    24,000/30,000  ( 80.0%)  2,615 replays/s
    27,000/30,000  ( 90.0%)  2,614 replays/s
    30,000/30,000  (100.0%)  2,611 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.250 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $      220
Median bucket bet              $                 10
Spread (max / min)                           22.00×
Buckets at table min                             65
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 10
TC +3                          $                 10
TC +4                          $                 10
TC +5                          $                 10
TC +6                          $                 10
TC +7                          $                 20
TC +8                          $                 20
TC +9                          $                 20
TC +10                         $                 20

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -46.65
P(finish down)  (end < B0)                   55.07%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              32.67%
P(end < 0.75·B0)                              9.37%
P(end < 0.5·B0)                               0.27%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,953.35
Median ending bankroll         $           1,955.00
95% range [p2.5 .. p97.5]      $ 1,275.00  ..  $ 2,630.00
P(lose > 25%)                                 9.37%
P(lose > 50%)                                 0.27%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             177.73
Median max drawdown            $             175.00
95th pct max drawdown          $             240.00
99th pct max drawdown          $             275.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.5300%
Avg session wagered            $           9,925.92

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              861.2
======================================================================

→ Kelly k=0.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,589 replays/s
     6,000/30,000  ( 20.0%)  2,594 replays/s
     9,000/30,000  ( 30.0%)  2,602 replays/s
    12,000/30,000  ( 40.0%)  2,605 replays/s
    15,000/30,000  ( 50.0%)  2,608 replays/s
    18,000/30,000  ( 60.0%)  2,610 replays/s
    21,000/30,000  ( 70.0%)  2,611 replays/s
    24,000/30,000  ( 80.0%)  2,612 replays/s
    27,000/30,000  ( 90.0%)  2,612 replays/s
    30,000/30,000  (100.0%)  2,613 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.500 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $      450
Median bucket bet              $                 10
Spread (max / min)                           45.00×
Buckets at table min                             60
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 10
TC +3                          $                 10
TC +4                          $                 10
TC +5                          $                 20
TC +6                          $                 30
TC +7                          $                 30
TC +8                          $                 40
TC +9                          $                 40
TC +10                         $                 40

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -32.39
P(finish down)  (end < B0)                   52.86%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              32.62%
P(end < 0.75·B0)                             10.57%
P(end < 0.5·B0)                               0.52%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,967.61
Median ending bankroll         $           1,970.00
95% range [p2.5 .. p97.5]      $ 1,225.00  ..  $ 2,705.12
P(lose > 25%)                                10.57%
P(lose > 50%)                                 0.52%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             205.04
Median max drawdown            $             195.00
95th pct max drawdown          $             315.00
99th pct max drawdown          $             405.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.6903%
Avg session wagered            $          10,456.61

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              861.2
======================================================================

→ Kelly k=0.750   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,608 replays/s
     6,000/30,000  ( 20.0%)  2,606 replays/s
     9,000/30,000  ( 30.0%)  2,610 replays/s
    12,000/30,000  ( 40.0%)  2,611 replays/s
    15,000/30,000  ( 50.0%)  2,611 replays/s
    18,000/30,000  ( 60.0%)  2,611 replays/s
    21,000/30,000  ( 70.0%)  2,611 replays/s
    24,000/30,000  ( 80.0%)  2,611 replays/s
    27,000/30,000  ( 90.0%)  2,610 replays/s
    30,000/30,000  (100.0%)  2,610 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.750 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $      670
Median bucket bet              $                 10
Spread (max / min)                           67.00×
Buckets at table min                             58
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 10
TC +3                          $                 10
TC +4                          $                 20
TC +5                          $                 30
TC +6                          $                 40
TC +7                          $                 50
TC +8                          $                 50
TC +9                          $                 60
TC +10                         $                 60

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -16.05
P(finish down)  (end < B0)                   50.96%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              33.23%
P(end < 0.75·B0)                             13.04%
P(end < 0.5·B0)                               1.31%
P(end < 0.25·B0)                              0.06%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           1,983.95
Median ending bankroll         $           1,990.00
95% range [p2.5 .. p97.5]      $ 1,125.00  ..  $ 2,840.00
P(lose > 25%)                                13.04%
P(lose > 50%)                                 1.31%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             256.42
Median max drawdown            $             230.00
95th pct max drawdown          $             445.00
99th pct max drawdown          $             585.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.8575%
Avg session wagered            $          11,265.36

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              861.2
======================================================================

→ Kelly k=1.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,616 replays/s
     6,000/30,000  ( 20.0%)  2,616 replays/s
     9,000/30,000  ( 30.0%)  2,617 replays/s
    12,000/30,000  ( 40.0%)  2,617 replays/s
    15,000/30,000  ( 50.0%)  2,617 replays/s
    18,000/30,000  ( 60.0%)  2,618 replays/s
    21,000/30,000  ( 70.0%)  2,619 replays/s
    24,000/30,000  ( 80.0%)  2,620 replays/s
    27,000/30,000  ( 90.0%)  2,620 replays/s
    30,000/30,000  (100.0%)  2,620 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $      900
Median bucket bet              $                 10
Spread (max / min)                           90.00×
Buckets at table min                             56
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 10
TC +3                          $                 20
TC +4                          $                 30
TC +5                          $                 40
TC +6                          $                 50
TC +7                          $                 60
TC +8                          $                 70
TC +9                          $                 80
TC +10                         $                 90

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $              +4.51
P(finish down)  (end < B0)                   49.17%
P(ruin)                                       0.05%
P(end < 0.9·B0)                              34.13%
P(end < 0.75·B0)                             16.07%
P(end < 0.5·B0)                               2.80%
P(end < 0.25·B0)                              0.30%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,004.51
Median ending bankroll         $           2,010.00
95% range [p2.5 .. p97.5]      $   970.00  ..  $ 3,030.00
P(lose > 25%)                                16.07%
P(lose > 50%)                                 2.80%
P(double bankroll)                            0.03%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             328.26
Median max drawdown            $             300.00
95th pct max drawdown          $             590.00
99th pct max drawdown          $             780.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.0360%
Avg session wagered            $          12,502.44

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              861.1
======================================================================

→ Kelly k=1.250   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,592 replays/s
     6,000/30,000  ( 20.0%)  2,595 replays/s
     9,000/30,000  ( 30.0%)  2,597 replays/s
    12,000/30,000  ( 40.0%)  2,595 replays/s
    15,000/30,000  ( 50.0%)  2,594 replays/s
    18,000/30,000  ( 60.0%)  2,597 replays/s
    21,000/30,000  ( 70.0%)  2,600 replays/s
    24,000/30,000  ( 80.0%)  2,599 replays/s
    27,000/30,000  ( 90.0%)  2,599 replays/s
    30,000/30,000  (100.0%)  2,600 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.250
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.250 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    1,120
Median bucket bet              $                 10
Spread (max / min)                          112.00×
Buckets at table min                             56
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 10
TC +3                          $                 20
TC +4                          $                 40
TC +5                          $                 50
TC +6                          $                 60
TC +7                          $                 80
TC +8                          $                 90
TC +9                          $                100
TC +10                         $                110

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +19.57
P(finish down)  (end < B0)                   48.22%
P(ruin)                                       0.19%
P(end < 0.9·B0)                              34.97%
P(end < 0.75·B0)                             17.96%
P(end < 0.5·B0)                               4.14%
P(end < 0.25·B0)                              0.72%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,019.57
Median ending bankroll         $           2,020.00
95% range [p2.5 .. p97.5]      $   849.88  ..  $ 3,180.00
P(lose > 25%)                                17.96%
P(lose > 50%)                                 4.14%
P(double bankroll)                            0.12%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             387.13
Median max drawdown            $             350.00
95th pct max drawdown          $             715.00
99th pct max drawdown          $             960.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.1482%
Avg session wagered            $          13,207.48

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              860.9
======================================================================

→ Kelly k=1.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,604 replays/s
     6,000/30,000  ( 20.0%)  2,609 replays/s
     9,000/30,000  ( 30.0%)  2,614 replays/s
    12,000/30,000  ( 40.0%)  2,615 replays/s
    15,000/30,000  ( 50.0%)  2,615 replays/s
    18,000/30,000  ( 60.0%)  2,616 replays/s
    21,000/30,000  ( 70.0%)  2,616 replays/s
    24,000/30,000  ( 80.0%)  2,615 replays/s
    27,000/30,000  ( 90.0%)  2,615 replays/s
    30,000/30,000  (100.0%)  2,617 replays/s
  done in 11.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.500 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    1,350
Median bucket bet              $                 10
Spread (max / min)                          135.00×
Buckets at table min                             55
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 20
TC +3                          $                 30
TC +4                          $                 40
TC +5                          $                 60
TC +6                          $                 80
TC +7                          $                 90
TC +8                          $                110
TC +9                          $                120
TC +10                         $                130

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +39.55
P(finish down)  (end < B0)                   47.31%
P(ruin)                                       0.60%
P(end < 0.9·B0)                              35.63%
P(end < 0.75·B0)                             20.51%
P(end < 0.5·B0)                               6.32%
P(end < 0.25·B0)                              1.60%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,039.55
Median ending bankroll         $           2,040.00
95% range [p2.5 .. p97.5]      $   655.00  ..  $ 3,405.00
P(lose > 25%)                                20.51%
P(lose > 50%)                                 6.32%
P(double bankroll)                            0.36%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             462.61
Median max drawdown            $             420.00
95th pct max drawdown          $             860.00
99th pct max drawdown          $           1,145.05

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.2719%
Avg session wagered            $          14,547.09

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               20.0
Avg hands played                              860.0
======================================================================

→ Kelly k=2.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,625 replays/s
     6,000/30,000  ( 20.0%)  2,633 replays/s
     9,000/30,000  ( 30.0%)  2,633 replays/s
    12,000/30,000  ( 40.0%)  2,634 replays/s
    15,000/30,000  ( 50.0%)  2,632 replays/s
    18,000/30,000  ( 60.0%)  2,632 replays/s
    21,000/30,000  ( 70.0%)  2,632 replays/s
    24,000/30,000  ( 80.0%)  2,632 replays/s
    27,000/30,000  ( 90.0%)  2,631 replays/s
    30,000/30,000  (100.0%)  2,629 replays/s
  done in 11.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    1,800
Median bucket bet              $                 10
Spread (max / min)                          180.00×
Buckets at table min                             55
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 20
TC +3                          $                 40
TC +4                          $                 60
TC +5                          $                 80
TC +6                          $                100
TC +7                          $                120
TC +8                          $                140
TC +9                          $                160
TC +10                         $                170

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +74.14
P(finish down)  (end < B0)                   46.11%
P(ruin)                                       2.25%
P(end < 0.9·B0)                              36.61%
P(end < 0.75·B0)                             24.21%
P(end < 0.5·B0)                              10.08%
P(end < 0.25·B0)                              3.98%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,074.14
Median ending bankroll         $           2,075.00
95% range [p2.5 .. p97.5]      $   169.88  ..  $ 3,815.00
P(lose > 25%)                                24.21%
P(lose > 50%)                                10.08%
P(double bankroll)                            1.59%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             602.44
Median max drawdown            $             550.00
95th pct max drawdown          $           1,125.00
99th pct max drawdown          $           1,480.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.4512%
Avg session wagered            $          16,433.88

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.9
Avg hands played                              855.8
======================================================================

→ Kelly k=2.500   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,649 replays/s
     6,000/30,000  ( 20.0%)  2,652 replays/s
     9,000/30,000  ( 30.0%)  2,651 replays/s
    12,000/30,000  ( 40.0%)  2,655 replays/s
    15,000/30,000  ( 50.0%)  2,657 replays/s
    18,000/30,000  ( 60.0%)  2,658 replays/s
    21,000/30,000  ( 70.0%)  2,658 replays/s
    24,000/30,000  ( 80.0%)  2,659 replays/s
    27,000/30,000  ( 90.0%)  2,660 replays/s
    30,000/30,000  (100.0%)  2,659 replays/s
  done in 11.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.500 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             55
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 30
TC +3                          $                 50
TC +4                          $                 70
TC +5                          $                 90
TC +6                          $                130
TC +7                          $                150
TC +8                          $                180
TC +9                          $                200
TC +10                         $                210

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +107.39
P(finish down)  (end < B0)                   45.50%
P(ruin)                                       5.10%
P(end < 0.9·B0)                              37.58%
P(end < 0.75·B0)                             27.08%
P(end < 0.5·B0)                              14.02%
P(end < 0.25·B0)                              7.28%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,107.39
Median ending bankroll         $           2,110.00
95% range [p2.5 .. p97.5]      $   -35.00  ..  $ 4,240.00
P(lose > 25%)                                27.08%
P(lose > 50%)                                14.02%
P(double bankroll)                            3.83%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             739.61
Median max drawdown            $             675.00
95th pct max drawdown          $           1,390.00
99th pct max drawdown          $           1,805.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.5826%
Avg session wagered            $          18,430.79

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.7
Avg hands played                              846.8
======================================================================

→ Kelly k=3.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,696 replays/s
     6,000/30,000  ( 20.0%)  2,693 replays/s
     9,000/30,000  ( 30.0%)  2,691 replays/s
    12,000/30,000  ( 40.0%)  2,690 replays/s
    15,000/30,000  ( 50.0%)  2,691 replays/s
    18,000/30,000  ( 60.0%)  2,691 replays/s
    21,000/30,000  ( 70.0%)  2,690 replays/s
    24,000/30,000  ( 80.0%)  2,690 replays/s
    27,000/30,000  ( 90.0%)  2,691 replays/s
    30,000/30,000  (100.0%)  2,692 replays/s
  done in 11.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=3.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             54
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 30
TC +3                          $                 60
TC +4                          $                 90
TC +5                          $                110
TC +6                          $                150
TC +7                          $                180
TC +8                          $                210
TC +9                          $                240
TC +10                         $                260

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +141.62
P(finish down)  (end < B0)                   45.11%
P(ruin)                                       8.86%
P(end < 0.9·B0)                              38.49%
P(end < 0.75·B0)                             29.48%
P(end < 0.5·B0)                              17.99%
P(end < 0.25·B0)                             11.07%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,141.62
Median ending bankroll         $           2,140.00
95% range [p2.5 .. p97.5]      $   -90.00  ..  $ 4,700.12
P(lose > 25%)                                29.48%
P(lose > 50%)                                17.99%
P(double bankroll)                            7.05%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             885.79
Median max drawdown            $             815.00
95th pct max drawdown          $           1,645.00
99th pct max drawdown          $           2,095.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.6878%
Avg session wagered            $          20,590.94

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               19.4
Avg hands played                              832.7
======================================================================

→ Kelly k=4.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,796 replays/s
     6,000/30,000  ( 20.0%)  2,784 replays/s
     9,000/30,000  ( 30.0%)  2,785 replays/s
    12,000/30,000  ( 40.0%)  2,789 replays/s
    15,000/30,000  ( 50.0%)  2,796 replays/s
    18,000/30,000  ( 60.0%)  2,798 replays/s
    21,000/30,000  ( 70.0%)  2,797 replays/s
    24,000/30,000  ( 80.0%)  2,800 replays/s
    27,000/30,000  ( 90.0%)  2,800 replays/s
    30,000/30,000  (100.0%)  2,800 replays/s
  done in 10.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=4.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             54
Buckets at table max                              3

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 40
TC +3                          $                 80
TC +4                          $                120
TC +5                          $                150
TC +6                          $                200
TC +7                          $                240
TC +8                          $                280
TC +9                          $                310
TC +10                         $                340

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +199.29
P(finish down)  (end < B0)                   45.20%
P(ruin)                                      16.72%
P(end < 0.9·B0)                              40.36%
P(end < 0.75·B0)                             33.60%
P(end < 0.5·B0)                              24.47%
P(end < 0.25·B0)                             18.87%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,199.29
Median ending bankroll         $           2,185.00
95% range [p2.5 .. p97.5]      $  -185.00  ..  $ 5,565.00
P(lose > 25%)                                33.60%
P(lose > 50%)                                24.47%
P(double bankroll)                           12.96%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,148.74
Median max drawdown            $           1,060.00
95th pct max drawdown          $           2,090.00
99th pct max drawdown          $           2,610.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.8522%
Avg session wagered            $          23,384.11

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               18.6
Avg hands played                              798.3
======================================================================

→ Kelly k=5.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  2,972 replays/s
     6,000/30,000  ( 20.0%)  2,962 replays/s
     9,000/30,000  ( 30.0%)  2,971 replays/s
    12,000/30,000  ( 40.0%)  2,955 replays/s
    15,000/30,000  ( 50.0%)  2,959 replays/s
    18,000/30,000  ( 60.0%)  2,960 replays/s
    21,000/30,000  ( 70.0%)  2,959 replays/s
    24,000/30,000  ( 80.0%)  2,963 replays/s
    27,000/30,000  ( 90.0%)  2,962 replays/s
    30,000/30,000  (100.0%)  2,961 replays/s
  done in 10.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 5.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=5.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             54
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 50
TC +3                          $                100
TC +4                          $                150
TC +5                          $                190
TC +6                          $                250
TC +7                          $                300
TC +8                          $                350
TC +9                          $                390
TC +10                         $                430

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +244.61
P(finish down)  (end < B0)                   46.45%
P(ruin)                                      25.13%
P(end < 0.9·B0)                              42.96%
P(end < 0.75·B0)                             37.93%
P(end < 0.5·B0)                              31.16%
P(end < 0.25·B0)                             26.94%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,244.61
Median ending bankroll         $           2,180.00
95% range [p2.5 .. p97.5]      $  -280.00  ..  $ 6,460.12
P(lose > 25%)                                37.93%
P(lose > 50%)                                31.16%
P(double bankroll)                           18.62%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,402.98
Median max drawdown            $           1,310.00
95th pct max drawdown          $           2,485.00
99th pct max drawdown          $           3,110.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.9318%
Avg session wagered            $          26,251.61

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               17.6
Avg hands played                              756.3
======================================================================

→ Kelly k=6.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  3,090 replays/s
     6,000/30,000  ( 20.0%)  3,073 replays/s
     9,000/30,000  ( 30.0%)  3,078 replays/s
    12,000/30,000  ( 40.0%)  3,072 replays/s
    15,000/30,000  ( 50.0%)  3,076 replays/s
    18,000/30,000  ( 60.0%)  3,081 replays/s
    21,000/30,000  ( 70.0%)  3,081 replays/s
    24,000/30,000  ( 80.0%)  3,087 replays/s
    27,000/30,000  ( 90.0%)  3,089 replays/s
    30,000/30,000  (100.0%)  3,091 replays/s
  done in 9.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 6.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=6.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             54
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 60
TC +3                          $                120
TC +4                          $                180
TC +5                          $                230
TC +6                          $                300
TC +7                          $                360
TC +8                          $                420
TC +9                          $                470
TC +10                         $                510

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +281.52
P(finish down)  (end < B0)                   48.51%
P(ruin)                                      32.11%
P(end < 0.9·B0)                              45.81%
P(end < 0.75·B0)                             41.97%
P(end < 0.5·B0)                              36.78%
P(end < 0.25·B0)                             33.39%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,281.52
Median ending bankroll         $           2,100.00
95% range [p2.5 .. p97.5]      $  -365.00  ..  $ 7,330.00
P(lose > 25%)                                41.97%
P(lose > 50%)                                36.78%
P(double bankroll)                           22.61%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,630.92
Median max drawdown            $           1,530.00
95th pct max drawdown          $           2,845.00
99th pct max drawdown          $           3,595.05

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0023%
Avg session wagered            $          28,087.23

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               16.8
Avg hands played                              718.8
======================================================================

→ Kelly k=7.000   (30,000 replays × 20 shoes)
     3,000/30,000  ( 10.0%)  3,261 replays/s
     6,000/30,000  ( 20.0%)  3,238 replays/s
     9,000/30,000  ( 30.0%)  3,242 replays/s
    12,000/30,000  ( 40.0%)  3,231 replays/s
    15,000/30,000  ( 50.0%)  3,246 replays/s
    18,000/30,000  ( 60.0%)  3,256 replays/s
    21,000/30,000  ( 70.0%)  3,258 replays/s
    24,000/30,000  ( 80.0%)  3,263 replays/s
    27,000/30,000  ( 90.0%)  3,263 replays/s
    30,000/30,000  (100.0%)  3,264 replays/s
  done in 9.2s

======================================================================
KELLY BANKROLL REPLAY  —  k = 7.000
======================================================================
Replays                                      30,000
Shoes per session                                20
Starting bankroll (B0)         $           2,000.00
Table min / max                 $10 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=7.000 | B0=$2,000 | min=$10 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       10  /  $    2,000
Median bucket bet              $                 10
Spread (max / min)                          200.00×
Buckets at table min                             54
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 10
TC +2                          $                 70
TC +3                          $                140
TC +4                          $                210
TC +5                          $                260
TC +6                          $                350
TC +7                          $                420
TC +8                          $                490
TC +9                          $                550
TC +10                         $                600

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +314.32
P(finish down)  (end < B0)                   50.89%
P(ruin)                                      38.16%
P(end < 0.9·B0)                              48.76%
P(end < 0.75·B0)                             45.85%
P(end < 0.5·B0)                              41.79%
P(end < 0.25·B0)                             39.31%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           2,314.32
Median ending bankroll         $           1,915.00
95% range [p2.5 .. p97.5]      $  -445.00  ..  $ 8,205.12
P(lose > 25%)                                45.85%
P(lose > 50%)                                41.79%
P(double bankroll)                           25.56%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,845.85
Median max drawdown            $           1,740.00
95th pct max drawdown          $           3,185.00
99th pct max drawdown          $           4,075.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0427%
Avg session wagered            $          30,144.59

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.9
Avg hands played                              680.8
======================================================================

Figure saved to: kelly_sweep_hilo_6d75pen_h17_br2000min10.png
