Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_2deck_70pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  142 TC buckets in [-36.0 .. +37.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_2deck_70pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 13,751,248 hands (avg 13.8 hands/shoe) in 2.9s

→ Kelly k=0.250   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,285 replays/s
     6,000/30,000  ( 20.0%)  5,267 replays/s
     9,000/30,000  ( 30.0%)  5,270 replays/s
    12,000/30,000  ( 40.0%)  5,284 replays/s
    15,000/30,000  ( 50.0%)  5,294 replays/s
    18,000/30,000  ( 60.0%)  5,305 replays/s
    21,000/30,000  ( 70.0%)  5,310 replays/s
    24,000/30,000  ( 80.0%)  5,313 replays/s
    27,000/30,000  ( 90.0%)  5,316 replays/s
    30,000/30,000  (100.0%)  5,311 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.250 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             93
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 25
TC +5                          $                 25
TC +6                          $                 25
TC +7                          $                 50
TC +8                          $                 50
TC +9                          $                 50
TC +10                         $                 50

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $              -1.89
P(finish down)  (end < B0)                   49.38%
P(ruin)                                       0.00%
P(end < 0.9·B0)                               8.47%
P(end < 0.75·B0)                              0.03%
P(end < 0.5·B0)                               0.00%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           4,998.11
Median ending bankroll         $           5,000.00
95% range [p2.5 .. p97.5]      $ 4,275.00  ..  $ 5,725.00
P(lose > 25%)                                 0.03%
P(lose > 50%)                                 0.00%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             225.11
Median max drawdown            $             225.00
95th pct max drawdown          $             350.00
99th pct max drawdown          $             437.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.9544%
Avg session wagered            $           4,152.76

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=0.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,296 replays/s
     6,000/30,000  ( 20.0%)  5,288 replays/s
     9,000/30,000  ( 30.0%)  5,294 replays/s
    12,000/30,000  ( 40.0%)  5,302 replays/s
    15,000/30,000  ( 50.0%)  5,305 replays/s
    18,000/30,000  ( 60.0%)  5,303 replays/s
    21,000/30,000  ( 70.0%)  5,304 replays/s
    24,000/30,000  ( 80.0%)  5,304 replays/s
    27,000/30,000  ( 90.0%)  5,306 replays/s
    30,000/30,000  (100.0%)  5,307 replays/s
  done in 5.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.500 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             87
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 50
TC +5                          $                 50
TC +6                          $                 75
TC +7                          $                 75
TC +8                          $                100
TC +9                          $                100
TC +10                         $                125

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +23.04
P(finish down)  (end < B0)                   47.23%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              13.47%
P(end < 0.75·B0)                              0.65%
P(end < 0.5·B0)                               0.00%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,023.04
Median ending bankroll         $           5,025.00
95% range [p2.5 .. p97.5]      $ 4,050.00  ..  $ 6,000.00
P(lose > 25%)                                 0.65%
P(lose > 50%)                                 0.00%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             320.87
Median max drawdown            $             287.50
95th pct max drawdown          $             600.00
99th pct max drawdown          $             800.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.4646%
Avg session wagered            $           4,959.24

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=0.750   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,282 replays/s
     6,000/30,000  ( 20.0%)  5,283 replays/s
     9,000/30,000  ( 30.0%)  5,290 replays/s
    12,000/30,000  ( 40.0%)  5,297 replays/s
    15,000/30,000  ( 50.0%)  5,304 replays/s
    18,000/30,000  ( 60.0%)  5,309 replays/s
    21,000/30,000  ( 70.0%)  5,311 replays/s
    24,000/30,000  ( 80.0%)  5,315 replays/s
    27,000/30,000  ( 90.0%)  5,319 replays/s
    30,000/30,000  (100.0%)  5,320 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.750 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             85
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 50
TC +4                          $                 75
TC +5                          $                 75
TC +6                          $                100
TC +7                          $                125
TC +8                          $                150
TC +9                          $                150
TC +10                         $                175

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +48.58
P(finish down)  (end < B0)                   46.30%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              18.63%
P(end < 0.75·B0)                              2.45%
P(end < 0.5·B0)                               0.02%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,048.58
Median ending bankroll         $           5,050.00
95% range [p2.5 .. p97.5]      $ 3,750.00  ..  $ 6,350.00
P(lose > 25%)                                 2.45%
P(lose > 50%)                                 0.02%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             438.29
Median max drawdown            $             387.50
95th pct max drawdown          $             850.00
99th pct max drawdown          $           1,162.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.8252%
Avg session wagered            $           5,887.03

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=1.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,321 replays/s
     6,000/30,000  ( 20.0%)  5,318 replays/s
     9,000/30,000  ( 30.0%)  5,315 replays/s
    12,000/30,000  ( 40.0%)  5,315 replays/s
    15,000/30,000  ( 50.0%)  5,318 replays/s
    18,000/30,000  ( 60.0%)  5,322 replays/s
    21,000/30,000  ( 70.0%)  5,324 replays/s
    24,000/30,000  ( 80.0%)  5,325 replays/s
    27,000/30,000  ( 90.0%)  5,325 replays/s
    30,000/30,000  (100.0%)  5,325 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             84
Buckets at table max                              1

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                 75
TC +4                          $                100
TC +5                          $                125
TC +6                          $                150
TC +7                          $                150
TC +8                          $                200
TC +9                          $                200
TC +10                         $                225

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +76.30
P(finish down)  (end < B0)                   45.55%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              22.80%
P(end < 0.75·B0)                              5.35%
P(end < 0.5·B0)                               0.26%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,076.30
Median ending bankroll         $           5,075.00
95% range [p2.5 .. p97.5]      $ 3,412.50  ..  $ 6,762.50
P(lose > 25%)                                 5.35%
P(lose > 50%)                                 0.26%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             572.63
Median max drawdown            $             500.00
95th pct max drawdown          $           1,125.00
99th pct max drawdown          $           1,537.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0971%
Avg session wagered            $           6,954.90

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=1.250   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,306 replays/s
     6,000/30,000  ( 20.0%)  5,325 replays/s
     9,000/30,000  ( 30.0%)  5,332 replays/s
    12,000/30,000  ( 40.0%)  5,334 replays/s
    15,000/30,000  ( 50.0%)  5,335 replays/s
    18,000/30,000  ( 60.0%)  5,333 replays/s
    21,000/30,000  ( 70.0%)  5,332 replays/s
    24,000/30,000  ( 80.0%)  5,331 replays/s
    27,000/30,000  ( 90.0%)  5,332 replays/s
    30,000/30,000  (100.0%)  5,333 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.250
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.250 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             83
Buckets at table max                              2

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                 75
TC +4                          $                125
TC +5                          $                150
TC +6                          $                175
TC +7                          $                200
TC +8                          $                225
TC +9                          $                275
TC +10                         $                300

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +103.36
P(finish down)  (end < B0)                   45.16%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              25.92%
P(end < 0.75·B0)                              8.43%
P(end < 0.5·B0)                               0.90%
P(end < 0.25·B0)                              0.05%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,103.36
Median ending bankroll         $           5,100.00
95% range [p2.5 .. p97.5]      $ 3,050.00  ..  $ 7,175.00
P(lose > 25%)                                 8.43%
P(lose > 50%)                                 0.90%
P(double bankroll)                            0.01%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             708.05
Median max drawdown            $             625.00
95th pct max drawdown          $           1,425.00
99th pct max drawdown          $           1,950.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.2819%
Avg session wagered            $           8,062.89

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=1.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,305 replays/s
     6,000/30,000  ( 20.0%)  5,321 replays/s
     9,000/30,000  ( 30.0%)  5,329 replays/s
    12,000/30,000  ( 40.0%)  5,332 replays/s
    15,000/30,000  ( 50.0%)  5,332 replays/s
    18,000/30,000  ( 60.0%)  5,336 replays/s
    21,000/30,000  ( 70.0%)  5,338 replays/s
    24,000/30,000  ( 80.0%)  5,336 replays/s
    27,000/30,000  ( 90.0%)  5,336 replays/s
    30,000/30,000  (100.0%)  5,335 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.500 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             83
Buckets at table max                              3

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                100
TC +4                          $                150
TC +5                          $                175
TC +6                          $                200
TC +7                          $                250
TC +8                          $                275
TC +9                          $                325
TC +10                         $                350

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +129.02
P(finish down)  (end < B0)                   44.99%
P(ruin)                                       0.04%
P(end < 0.9·B0)                              28.08%
P(end < 0.75·B0)                             11.35%
P(end < 0.5·B0)                               1.77%
P(end < 0.25·B0)                              0.21%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,129.02
Median ending bankroll         $           5,125.00
95% range [p2.5 .. p97.5]      $ 2,725.00  ..  $ 7,575.00
P(lose > 25%)                                11.35%
P(lose > 50%)                                 1.77%
P(double bankroll)                            0.03%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             837.11
Median max drawdown            $             750.00
95th pct max drawdown          $           1,675.00
99th pct max drawdown          $           2,312.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.4402%
Avg session wagered            $           8,958.19

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.5
======================================================================

→ Kelly k=2.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,301 replays/s
     6,000/30,000  ( 20.0%)  5,299 replays/s
     9,000/30,000  ( 30.0%)  5,300 replays/s
    12,000/30,000  ( 40.0%)  5,296 replays/s
    15,000/30,000  ( 50.0%)  5,296 replays/s
    18,000/30,000  ( 60.0%)  5,294 replays/s
    21,000/30,000  ( 70.0%)  5,296 replays/s
    24,000/30,000  ( 80.0%)  5,296 replays/s
    27,000/30,000  ( 90.0%)  5,295 replays/s
    30,000/30,000  (100.0%)  5,295 replays/s
  done in 5.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             83
Buckets at table max                              7

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 75
TC +3                          $                150
TC +4                          $                175
TC +5                          $                225
TC +6                          $                275
TC +7                          $                325
TC +8                          $                375
TC +9                          $                425
TC +10                         $                450

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +178.62
P(finish down)  (end < B0)                   44.81%
P(ruin)                                       0.31%
P(end < 0.9·B0)                              31.35%
P(end < 0.75·B0)                             16.11%
P(end < 0.5·B0)                               4.25%
P(end < 0.25·B0)                              1.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,178.62
Median ending bankroll         $           5,175.00
95% range [p2.5 .. p97.5]      $ 2,037.50  ..  $ 8,362.81
P(lose > 25%)                                16.11%
P(lose > 50%)                                 4.25%
P(double bankroll)                            0.33%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,089.76
Median max drawdown            $             975.00
95th pct max drawdown          $           2,225.00
99th pct max drawdown          $           3,050.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.6514%
Avg session wagered            $          10,816.36

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.4
======================================================================

→ Kelly k=2.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,303 replays/s
     6,000/30,000  ( 20.0%)  5,332 replays/s
     9,000/30,000  ( 30.0%)  5,336 replays/s
    12,000/30,000  ( 40.0%)  5,336 replays/s
    15,000/30,000  ( 50.0%)  5,338 replays/s
    18,000/30,000  ( 60.0%)  5,335 replays/s
    21,000/30,000  ( 70.0%)  5,336 replays/s
    24,000/30,000  ( 80.0%)  5,335 replays/s
    27,000/30,000  ( 90.0%)  5,336 replays/s
    30,000/30,000  (100.0%)  5,334 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.500 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                              8

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 50
TC +2                          $                100
TC +3                          $                175
TC +4                          $                225
TC +5                          $                300
TC +6                          $                350
TC +7                          $                400
TC +8                          $                475
TC +9                          $                525
TC +10                         $                575

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +235.32
P(finish down)  (end < B0)                   44.49%
P(ruin)                                       1.21%
P(end < 0.9·B0)                              33.86%
P(end < 0.75·B0)                             20.45%
P(end < 0.5·B0)                               7.56%
P(end < 0.25·B0)                              2.60%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,235.32
Median ending bankroll         $           5,231.25
95% range [p2.5 .. p97.5]      $ 1,187.50  ..  $ 9,262.50
P(lose > 25%)                                20.45%
P(lose > 50%)                                 7.56%
P(double bankroll)                            1.20%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,381.93
Median max drawdown            $           1,225.00
95th pct max drawdown          $           2,800.00
99th pct max drawdown          $           3,787.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.7734%
Avg session wagered            $          13,269.47

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              137.0
======================================================================

→ Kelly k=3.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,312 replays/s
     6,000/30,000  ( 20.0%)  5,315 replays/s
     9,000/30,000  ( 30.0%)  5,303 replays/s
    12,000/30,000  ( 40.0%)  5,319 replays/s
    15,000/30,000  ( 50.0%)  5,329 replays/s
    18,000/30,000  ( 60.0%)  5,329 replays/s
    21,000/30,000  ( 70.0%)  5,336 replays/s
    24,000/30,000  ( 80.0%)  5,339 replays/s
    27,000/30,000  ( 90.0%)  5,344 replays/s
    30,000/30,000  (100.0%)  5,348 replays/s
  done in 5.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=3.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                              9

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 50
TC +2                          $                125
TC +3                          $                200
TC +4                          $                275
TC +5                          $                350
TC +6                          $                425
TC +7                          $                475
TC +8                          $                575
TC +9                          $                625
TC +10                         $                700

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +284.06
P(finish down)  (end < B0)                   44.36%
P(ruin)                                       2.83%
P(end < 0.9·B0)                              35.28%
P(end < 0.75·B0)                             23.31%
P(end < 0.5·B0)                              10.75%
P(end < 0.25·B0)                              4.74%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,284.06
Median ending bankroll         $           5,275.00
95% range [p2.5 .. p97.5]      $   -12.50  ..  $10,062.50
P(lose > 25%)                                23.31%
P(lose > 50%)                                10.75%
P(double bankroll)                            2.64%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,638.80
Median max drawdown            $           1,450.00
95th pct max drawdown          $           3,325.00
99th pct max drawdown          $           4,500.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.8710%
Avg session wagered            $          15,182.40

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.9
Avg hands played                              136.2
======================================================================

→ Kelly k=4.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,432 replays/s
     6,000/30,000  ( 20.0%)  5,450 replays/s
     9,000/30,000  ( 30.0%)  5,442 replays/s
    12,000/30,000  ( 40.0%)  5,449 replays/s
    15,000/30,000  ( 50.0%)  5,452 replays/s
    18,000/30,000  ( 60.0%)  5,438 replays/s
    21,000/30,000  ( 70.0%)  5,444 replays/s
    24,000/30,000  ( 80.0%)  5,445 replays/s
    27,000/30,000  ( 90.0%)  5,451 replays/s
    30,000/30,000  (100.0%)  5,457 replays/s
  done in 5.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=4.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                             16

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 75
TC +2                          $                150
TC +3                          $                275
TC +4                          $                375
TC +5                          $                475
TC +6                          $                550
TC +7                          $                650
TC +8                          $                750
TC +9                          $                850
TC +10                         $                925

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +383.20
P(finish down)  (end < B0)                   44.51%
P(ruin)                                       7.60%
P(end < 0.9·B0)                              37.41%
P(end < 0.75·B0)                             28.33%
P(end < 0.5·B0)                              16.63%
P(end < 0.25·B0)                             10.20%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,383.20
Median ending bankroll         $           5,375.00
95% range [p2.5 .. p97.5]      $  -400.00  ..  $11,787.50
P(lose > 25%)                                28.33%
P(lose > 50%)                                16.63%
P(double bankroll)                            6.84%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,175.38
Median max drawdown            $           1,950.00
95th pct max drawdown          $           4,337.50
99th pct max drawdown          $           5,687.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       102.0017%
Avg session wagered            $          19,143.67

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.7
Avg hands played                              133.6
======================================================================

→ Kelly k=5.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,574 replays/s
     6,000/30,000  ( 20.0%)  5,580 replays/s
     9,000/30,000  ( 30.0%)  5,571 replays/s
    12,000/30,000  ( 40.0%)  5,572 replays/s
    15,000/30,000  ( 50.0%)  5,575 replays/s
    18,000/30,000  ( 60.0%)  5,583 replays/s
    21,000/30,000  ( 70.0%)  5,587 replays/s
    24,000/30,000  ( 80.0%)  5,587 replays/s
    27,000/30,000  ( 90.0%)  5,591 replays/s
    30,000/30,000  (100.0%)  5,598 replays/s
  done in 5.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 5.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=5.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                             24

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 75
TC +2                          $                200
TC +3                          $                350
TC +4                          $                450
TC +5                          $                575
TC +6                          $                700
TC +7                          $                800
TC +8                          $                950
TC +9                          $              1,050
TC +10                         $              1,150

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +461.86
P(finish down)  (end < B0)                   44.87%
P(ruin)                                      13.12%
P(end < 0.9·B0)                              39.38%
P(end < 0.75·B0)                             32.05%
P(end < 0.5·B0)                              22.07%
P(end < 0.25·B0)                             15.98%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,461.86
Median ending bankroll         $           5,437.50
95% range [p2.5 .. p97.5]      $  -750.00  ..  $13,437.81
P(lose > 25%)                                32.05%
P(lose > 50%)                                22.07%
P(double bankroll)                           11.20%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,665.56
Median max drawdown            $           2,425.00
95th pct max drawdown          $           5,250.62
99th pct max drawdown          $           6,737.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       102.0725%
Avg session wagered            $          22,285.82

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.5
Avg hands played                              130.0
======================================================================

→ Kelly k=6.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,744 replays/s
     6,000/30,000  ( 20.0%)  5,758 replays/s
     9,000/30,000  ( 30.0%)  5,746 replays/s
    12,000/30,000  ( 40.0%)  5,753 replays/s
    15,000/30,000  ( 50.0%)  5,757 replays/s
    18,000/30,000  ( 60.0%)  5,763 replays/s
    21,000/30,000  ( 70.0%)  5,769 replays/s
    24,000/30,000  ( 80.0%)  5,765 replays/s
    27,000/30,000  ( 90.0%)  5,769 replays/s
    30,000/30,000  (100.0%)  5,772 replays/s
  done in 5.2s

======================================================================
KELLY BANKROLL REPLAY  —  k = 6.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=6.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                             36

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                225
TC +3                          $                425
TC +4                          $                550
TC +5                          $                700
TC +6                          $                825
TC +7                          $                975
TC +8                          $              1,125
TC +9                          $              1,275
TC +10                         $              1,375

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +531.57
P(finish down)  (end < B0)                   45.63%
P(ruin)                                      18.82%
P(end < 0.9·B0)                              41.18%
P(end < 0.75·B0)                             35.20%
P(end < 0.5·B0)                              26.78%
P(end < 0.25·B0)                             21.43%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,531.57
Median ending bankroll         $           5,450.00
95% range [p2.5 .. p97.5]      $-1,050.00  ..  $15,087.50
P(lose > 25%)                                35.20%
P(lose > 50%)                                26.78%
P(double bankroll)                           15.41%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           3,137.18
Median max drawdown            $           2,875.00
95th pct max drawdown          $           6,000.00
99th pct max drawdown          $           7,712.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       102.1088%
Avg session wagered            $          25,207.60

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.2
Avg hands played                              125.9
======================================================================

→ Kelly k=7.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,888 replays/s
     6,000/30,000  ( 20.0%)  5,919 replays/s
     9,000/30,000  ( 30.0%)  5,912 replays/s
    12,000/30,000  ( 40.0%)  5,920 replays/s
    15,000/30,000  ( 50.0%)  5,925 replays/s
    18,000/30,000  ( 60.0%)  5,928 replays/s
    21,000/30,000  ( 70.0%)  5,935 replays/s
    24,000/30,000  ( 80.0%)  5,932 replays/s
    27,000/30,000  ( 90.0%)  5,934 replays/s
    30,000/30,000  (100.0%)  5,937 replays/s
  done in 5.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 7.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=7.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -36.0 ..     +37.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             82
Buckets at table max                             42

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                275
TC +3                          $                475
TC +4                          $                650
TC +5                          $                825
TC +6                          $                975
TC +7                          $              1,125
TC +8                          $              1,325
TC +9                          $              1,475
TC +10                         $              1,600

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +602.47
P(finish down)  (end < B0)                   46.59%
P(ruin)                                      24.00%
P(end < 0.9·B0)                              42.96%
P(end < 0.75·B0)                             37.96%
P(end < 0.5·B0)                              30.94%
P(end < 0.25·B0)                             26.38%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,602.47
Median ending bankroll         $           5,425.00
95% range [p2.5 .. p97.5]      $-1,325.00  ..  $16,637.81
P(lose > 25%)                                37.96%
P(lose > 50%)                                30.94%
P(double bankroll)                           19.04%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           3,572.26
Median max drawdown            $           3,312.50
95th pct max drawdown          $           6,700.00
99th pct max drawdown          $           8,587.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       102.1750%
Avg session wagered            $          27,699.33

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.0
Avg hands played                              121.8
======================================================================

Figure saved to: kelly_sweep_hilo_2d70pen_h17_br5000min25.png
