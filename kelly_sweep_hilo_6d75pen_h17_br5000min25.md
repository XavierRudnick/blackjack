Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  90 TC buckets in [-23.0 .. +23.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 43,061,863 hands (avg 43.1 hands/shoe) in 9.2s

→ Kelly k=0.250   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,486 replays/s
     6,000/30,000  ( 20.0%)  3,482 replays/s
     9,000/30,000  ( 30.0%)  3,483 replays/s
    12,000/30,000  ( 40.0%)  3,487 replays/s
    15,000/30,000  ( 50.0%)  3,486 replays/s
    18,000/30,000  ( 60.0%)  3,486 replays/s
    21,000/30,000  ( 70.0%)  3,486 replays/s
    24,000/30,000  ( 80.0%)  3,486 replays/s
    27,000/30,000  ( 90.0%)  3,485 replays/s
    30,000/30,000  (100.0%)  3,483 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.250 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $      550
Median bucket bet              $                 25
Spread (max / min)                           22.00×
Buckets at table min                             65
Buckets at table max                              0

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
E[profit]  = E[end − B0]       $             -86.78
P(finish down)  (end < B0)                   54.39%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              28.72%
P(end < 0.75·B0)                              5.74%
P(end < 0.5·B0)                               0.04%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           4,913.22
Median ending bankroll         $           4,912.50
95% range [p2.5 .. p97.5]      $ 3,450.00  ..  $ 6,375.00
P(lose > 25%)                                 5.74%
P(lose > 50%)                                 0.04%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             423.67
Median max drawdown            $             412.50
95th pct max drawdown          $             575.00
99th pct max drawdown          $             675.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.5337%
Avg session wagered            $          18,611.42

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.9
======================================================================

→ Kelly k=0.500   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,481 replays/s
     6,000/30,000  ( 20.0%)  3,475 replays/s
     9,000/30,000  ( 30.0%)  3,474 replays/s
    12,000/30,000  ( 40.0%)  3,473 replays/s
    15,000/30,000  ( 50.0%)  3,472 replays/s
    18,000/30,000  ( 60.0%)  3,472 replays/s
    21,000/30,000  ( 70.0%)  3,471 replays/s
    24,000/30,000  ( 80.0%)  3,471 replays/s
    27,000/30,000  ( 90.0%)  3,470 replays/s
    30,000/30,000  (100.0%)  3,470 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.500 | B0=$5,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $             -59.33
P(finish down)  (end < B0)                   52.43%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              29.19%
P(end < 0.75·B0)                              7.14%
P(end < 0.5·B0)                               0.17%
P(end < 0.25·B0)                              0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           4,940.67
Median ending bankroll         $           4,950.00
95% range [p2.5 .. p97.5]      $ 3,325.00  ..  $ 6,537.50
P(lose > 25%)                                 7.14%
P(lose > 50%)                                 0.17%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             482.82
Median max drawdown            $             450.00
95th pct max drawdown          $             750.00
99th pct max drawdown          $             975.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.6974%
Avg session wagered            $          19,605.42

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.9
======================================================================

→ Kelly k=0.750   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,470 replays/s
     6,000/30,000  ( 20.0%)  3,473 replays/s
     9,000/30,000  ( 30.0%)  3,482 replays/s
    12,000/30,000  ( 40.0%)  3,487 replays/s
    15,000/30,000  ( 50.0%)  3,490 replays/s
    18,000/30,000  ( 60.0%)  3,494 replays/s
    21,000/30,000  ( 70.0%)  3,496 replays/s
    24,000/30,000  ( 80.0%)  3,496 replays/s
    27,000/30,000  ( 90.0%)  3,496 replays/s
    30,000/30,000  (100.0%)  3,497 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.750 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    1,675
Median bucket bet              $                 25
Spread (max / min)                           67.00×
Buckets at table min                             58
Buckets at table max                              0

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 25
TC +3                          $                 25
TC +4                          $                 50
TC +5                          $                 75
TC +6                          $                100
TC +7                          $                125
TC +8                          $                125
TC +9                          $                150
TC +10                         $                150

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             -28.09
P(finish down)  (end < B0)                   50.84%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              30.15%
P(end < 0.75·B0)                              9.38%
P(end < 0.5·B0)                               0.51%
P(end < 0.25·B0)                              0.04%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           4,971.91
Median ending bankroll         $           4,975.00
95% range [p2.5 .. p97.5]      $ 3,100.00  ..  $ 6,812.50
P(lose > 25%)                                 9.38%
P(lose > 50%)                                 0.51%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             593.54
Median max drawdown            $             537.50
95th pct max drawdown          $           1,050.00
99th pct max drawdown          $           1,387.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.8670%
Avg session wagered            $          21,121.54

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.9
======================================================================

→ Kelly k=1.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,476 replays/s
     6,000/30,000  ( 20.0%)  3,476 replays/s
     9,000/30,000  ( 30.0%)  3,479 replays/s
    12,000/30,000  ( 40.0%)  3,480 replays/s
    15,000/30,000  ( 50.0%)  3,482 replays/s
    18,000/30,000  ( 60.0%)  3,487 replays/s
    21,000/30,000  ( 70.0%)  3,492 replays/s
    24,000/30,000  ( 80.0%)  3,492 replays/s
    27,000/30,000  ( 90.0%)  3,494 replays/s
    30,000/30,000  (100.0%)  3,496 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.000 | B0=$5,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $             +10.21
P(finish down)  (end < B0)                   49.55%
P(ruin)                                       0.04%
P(end < 0.9·B0)                              31.63%
P(end < 0.75·B0)                             12.63%
P(end < 0.5·B0)                               1.46%
P(end < 0.25·B0)                              0.14%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,010.21
Median ending bankroll         $           5,012.50
95% range [p2.5 .. p97.5]      $ 2,762.50  ..  $ 7,237.50
P(lose > 25%)                                12.63%
P(lose > 50%)                                 1.46%
P(double bankroll)                            0.01%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             752.14
Median max drawdown            $             675.00
95th pct max drawdown          $           1,387.50
99th pct max drawdown          $           1,837.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.0436%
Avg session wagered            $          23,442.01

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.9
======================================================================

→ Kelly k=1.250   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,486 replays/s
     6,000/30,000  ( 20.0%)  3,487 replays/s
     9,000/30,000  ( 30.0%)  3,491 replays/s
    12,000/30,000  ( 40.0%)  3,496 replays/s
    15,000/30,000  ( 50.0%)  3,501 replays/s
    18,000/30,000  ( 60.0%)  3,503 replays/s
    21,000/30,000  ( 70.0%)  3,505 replays/s
    24,000/30,000  ( 80.0%)  3,504 replays/s
    27,000/30,000  ( 90.0%)  3,504 replays/s
    30,000/30,000  (100.0%)  3,505 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.250
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.250 | B0=$5,000 | min=$25 / max=$2000

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
TC +7                          $                200
TC +8                          $                225
TC +9                          $                250
TC +10                         $                275

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +38.87
P(finish down)  (end < B0)                   48.71%
P(ruin)                                       0.12%
P(end < 0.9·B0)                              32.68%
P(end < 0.75·B0)                             14.77%
P(end < 0.5·B0)                               2.54%
P(end < 0.25·B0)                              0.34%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,038.87
Median ending bankroll         $           5,025.00
95% range [p2.5 .. p97.5]      $ 2,487.50  ..  $ 7,587.50
P(lose > 25%)                                14.77%
P(lose > 50%)                                 2.54%
P(double bankroll)                            0.05%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             880.58
Median max drawdown            $             787.50
95th pct max drawdown          $           1,700.00
99th pct max drawdown          $           2,250.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.1570%
Avg session wagered            $          24,766.25

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.8
======================================================================

→ Kelly k=1.500   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,487 replays/s
     6,000/30,000  ( 20.0%)  3,485 replays/s
     9,000/30,000  ( 30.0%)  3,486 replays/s
    12,000/30,000  ( 40.0%)  3,488 replays/s
    15,000/30,000  ( 50.0%)  3,489 replays/s
    18,000/30,000  ( 60.0%)  3,489 replays/s
    21,000/30,000  ( 70.0%)  3,491 replays/s
    24,000/30,000  ( 80.0%)  3,491 replays/s
    27,000/30,000  ( 90.0%)  3,494 replays/s
    30,000/30,000  (100.0%)  3,496 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.500 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             55
Buckets at table max                              2

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 50
TC +3                          $                 75
TC +4                          $                100
TC +5                          $                150
TC +6                          $                200
TC +7                          $                225
TC +8                          $                275
TC +9                          $                300
TC +10                         $                325

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $             +78.02
P(finish down)  (end < B0)                   47.69%
P(ruin)                                       0.27%
P(end < 0.9·B0)                              33.96%
P(end < 0.75·B0)                             17.37%
P(end < 0.5·B0)                               4.17%
P(end < 0.25·B0)                              0.80%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,078.02
Median ending bankroll         $           5,075.00
95% range [p2.5 .. p97.5]      $ 2,087.50  ..  $ 8,087.50
P(lose > 25%)                                17.37%
P(lose > 50%)                                 4.17%
P(double bankroll)                            0.17%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,049.39
Median max drawdown            $             950.00
95th pct max drawdown          $           2,025.00
99th pct max drawdown          $           2,687.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.2859%
Avg session wagered            $          27,294.55

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              645.5
======================================================================

→ Kelly k=2.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,499 replays/s
     6,000/30,000  ( 20.0%)  3,496 replays/s
     9,000/30,000  ( 30.0%)  3,502 replays/s
    12,000/30,000  ( 40.0%)  3,493 replays/s
    15,000/30,000  ( 50.0%)  3,496 replays/s
    18,000/30,000  ( 60.0%)  3,497 replays/s
    21,000/30,000  ( 70.0%)  3,500 replays/s
    24,000/30,000  ( 80.0%)  3,502 replays/s
    27,000/30,000  ( 90.0%)  3,503 replays/s
    30,000/30,000  (100.0%)  3,504 replays/s
  done in 8.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.000 | B0=$5,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +142.93
P(finish down)  (end < B0)                   46.79%
P(ruin)                                       1.12%
P(end < 0.9·B0)                              35.61%
P(end < 0.75·B0)                             21.25%
P(end < 0.5·B0)                               7.51%
P(end < 0.25·B0)                              2.39%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,142.93
Median ending bankroll         $           5,125.00
95% range [p2.5 .. p97.5]      $ 1,300.00  ..  $ 9,012.50
P(lose > 25%)                                21.25%
P(lose > 50%)                                 7.51%
P(double bankroll)                            0.87%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,362.91
Median max drawdown            $           1,225.00
95th pct max drawdown          $           2,662.50
99th pct max drawdown          $           3,525.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.4624%
Avg session wagered            $          30,907.71

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               15.0
Avg hands played                              643.7
======================================================================

→ Kelly k=2.500   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,512 replays/s
     6,000/30,000  ( 20.0%)  3,515 replays/s
     9,000/30,000  ( 30.0%)  3,514 replays/s
    12,000/30,000  ( 40.0%)  3,515 replays/s
    15,000/30,000  ( 50.0%)  3,521 replays/s
    18,000/30,000  ( 60.0%)  3,525 replays/s
    21,000/30,000  ( 70.0%)  3,528 replays/s
    24,000/30,000  ( 80.0%)  3,527 replays/s
    27,000/30,000  ( 90.0%)  3,526 replays/s
    30,000/30,000  (100.0%)  3,528 replays/s
  done in 8.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.500 | B0=$5,000 | min=$25 / max=$2000

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
TC +2                          $                 75
TC +3                          $                125
TC +4                          $                175
TC +5                          $                225
TC +6                          $                325
TC +7                          $                375
TC +8                          $                450
TC +9                          $                500
TC +10                         $                525

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +207.88
P(finish down)  (end < B0)                   46.08%
P(ruin)                                       2.94%
P(end < 0.9·B0)                              36.82%
P(end < 0.75·B0)                             24.47%
P(end < 0.5·B0)                              10.99%
P(end < 0.25·B0)                              4.69%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,207.88
Median ending bankroll         $           5,187.50
95% range [p2.5 .. p97.5]      $     0.00  ..  $ 9,912.81
P(lose > 25%)                                24.47%
P(lose > 50%)                                10.99%
P(double bankroll)                            2.35%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,673.04
Median max drawdown            $           1,500.00
95th pct max drawdown          $           3,275.00
99th pct max drawdown          $           4,287.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.5972%
Avg session wagered            $          34,812.15

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               14.9
Avg hands played                              639.5
======================================================================

→ Kelly k=3.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,561 replays/s
     6,000/30,000  ( 20.0%)  3,570 replays/s
     9,000/30,000  ( 30.0%)  3,562 replays/s
    12,000/30,000  ( 40.0%)  3,565 replays/s
    15,000/30,000  ( 50.0%)  3,566 replays/s
    18,000/30,000  ( 60.0%)  3,566 replays/s
    21,000/30,000  ( 70.0%)  3,566 replays/s
    24,000/30,000  ( 80.0%)  3,565 replays/s
    27,000/30,000  ( 90.0%)  3,564 replays/s
    30,000/30,000  (100.0%)  3,563 replays/s
  done in 8.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=3.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                              5

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                 75
TC +3                          $                150
TC +4                          $                225
TC +5                          $                275
TC +6                          $                375
TC +7                          $                450
TC +8                          $                525
TC +9                          $                600
TC +10                         $                650

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +274.16
P(finish down)  (end < B0)                   45.58%
P(ruin)                                       5.64%
P(end < 0.9·B0)                              37.85%
P(end < 0.75·B0)                             27.28%
P(end < 0.5·B0)                              14.53%
P(end < 0.25·B0)                              7.75%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,274.16
Median ending bankroll         $           5,262.50
95% range [p2.5 .. p97.5]      $  -137.50  ..  $10,900.31
P(lose > 25%)                                27.28%
P(lose > 50%)                                14.53%
P(double bankroll)                            4.50%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,006.51
Median max drawdown            $           1,825.00
95th pct max drawdown          $           3,887.50
99th pct max drawdown          $           5,012.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.7003%
Avg session wagered            $          39,149.67

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               14.7
Avg hands played                              632.9
======================================================================

→ Kelly k=4.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,665 replays/s
     6,000/30,000  ( 20.0%)  3,683 replays/s
     9,000/30,000  ( 30.0%)  3,680 replays/s
    12,000/30,000  ( 40.0%)  3,678 replays/s
    15,000/30,000  ( 50.0%)  3,674 replays/s
    18,000/30,000  ( 60.0%)  3,671 replays/s
    21,000/30,000  ( 70.0%)  3,671 replays/s
    24,000/30,000  ( 80.0%)  3,673 replays/s
    27,000/30,000  ( 90.0%)  3,673 replays/s
    30,000/30,000  (100.0%)  3,674 replays/s
  done in 8.2s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=4.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                              5

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                100
TC +3                          $                200
TC +4                          $                300
TC +5                          $                375
TC +6                          $                500
TC +7                          $                600
TC +8                          $                700
TC +9                          $                775
TC +10                         $                850

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +382.13
P(finish down)  (end < B0)                   45.45%
P(ruin)                                      12.18%
P(end < 0.9·B0)                              39.46%
P(end < 0.75·B0)                             31.51%
P(end < 0.5·B0)                              20.57%
P(end < 0.25·B0)                             14.49%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,382.13
Median ending bankroll         $           5,350.00
95% range [p2.5 .. p97.5]      $  -400.00  ..  $12,800.00
P(lose > 25%)                                31.51%
P(lose > 50%)                                20.57%
P(double bankroll)                            9.37%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,606.33
Median max drawdown            $           2,387.50
95th pct max drawdown          $           5,000.00
99th pct max drawdown          $           6,275.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.8502%
Avg session wagered            $          44,944.21

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               14.3
Avg hands played                              613.4
======================================================================

→ Kelly k=5.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,805 replays/s
     6,000/30,000  ( 20.0%)  3,806 replays/s
     9,000/30,000  ( 30.0%)  3,797 replays/s
    12,000/30,000  ( 40.0%)  3,805 replays/s
    15,000/30,000  ( 50.0%)  3,806 replays/s
    18,000/30,000  ( 60.0%)  3,805 replays/s
    21,000/30,000  ( 70.0%)  3,807 replays/s
    24,000/30,000  ( 80.0%)  3,813 replays/s
    27,000/30,000  ( 90.0%)  3,813 replays/s
    30,000/30,000  (100.0%)  3,811 replays/s
  done in 7.9s

======================================================================
KELLY BANKROLL REPLAY  —  k = 5.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=5.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                              8

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                125
TC +3                          $                250
TC +4                          $                375
TC +5                          $                475
TC +6                          $                625
TC +7                          $                750
TC +8                          $                875
TC +9                          $                975
TC +10                         $              1,075

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +480.91
P(finish down)  (end < B0)                   46.08%
P(ruin)                                      19.49%
P(end < 0.9·B0)                              41.43%
P(end < 0.75·B0)                             35.33%
P(end < 0.5·B0)                              26.64%
P(end < 0.25·B0)                             21.48%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,480.91
Median ending bankroll         $           5,400.00
95% range [p2.5 .. p97.5]      $  -625.00  ..  $14,812.50
P(lose > 25%)                                35.33%
P(lose > 50%)                                26.64%
P(double bankroll)                           14.28%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           3,193.78
Median max drawdown            $           2,950.00
95th pct max drawdown          $           5,900.00
99th pct max drawdown          $           7,412.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.9416%
Avg session wagered            $          51,075.41

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               13.7
Avg hands played                              589.0
======================================================================

→ Kelly k=6.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  3,944 replays/s
     6,000/30,000  ( 20.0%)  3,956 replays/s
     9,000/30,000  ( 30.0%)  3,956 replays/s
    12,000/30,000  ( 40.0%)  3,956 replays/s
    15,000/30,000  ( 50.0%)  3,958 replays/s
    18,000/30,000  ( 60.0%)  3,959 replays/s
    21,000/30,000  ( 70.0%)  3,960 replays/s
    24,000/30,000  ( 80.0%)  3,961 replays/s
    27,000/30,000  ( 90.0%)  3,959 replays/s
    30,000/30,000  (100.0%)  3,960 replays/s
  done in 7.6s

======================================================================
KELLY BANKROLL REPLAY  —  k = 6.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=6.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                             10

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                150
TC +3                          $                300
TC +4                          $                450
TC +5                          $                575
TC +6                          $                750
TC +7                          $                900
TC +8                          $              1,050
TC +9                          $              1,175
TC +10                         $              1,275

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +567.45
P(finish down)  (end < B0)                   47.18%
P(ruin)                                      25.77%
P(end < 0.9·B0)                              43.57%
P(end < 0.75·B0)                             38.78%
P(end < 0.5·B0)                              31.96%
P(end < 0.25·B0)                             27.42%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,567.45
Median ending bankroll         $           5,337.50
95% range [p2.5 .. p97.5]      $  -837.50  ..  $16,712.81
P(lose > 25%)                                38.78%
P(lose > 50%)                                31.96%
P(double bankroll)                           18.30%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           3,721.51
Median max drawdown            $           3,462.50
95th pct max drawdown          $           6,750.00
99th pct max drawdown          $           8,525.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0282%
Avg session wagered            $          55,188.90

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               13.2
Avg hands played                              565.6
======================================================================

→ Kelly k=7.000   (30,000 replays × 15 shoes)
     3,000/30,000  ( 10.0%)  4,113 replays/s
     6,000/30,000  ( 20.0%)  4,121 replays/s
     9,000/30,000  ( 30.0%)  4,120 replays/s
    12,000/30,000  ( 40.0%)  4,123 replays/s
    15,000/30,000  ( 50.0%)  4,124 replays/s
    18,000/30,000  ( 60.0%)  4,126 replays/s
    21,000/30,000  ( 70.0%)  4,127 replays/s
    24,000/30,000  ( 80.0%)  4,131 replays/s
    27,000/30,000  ( 90.0%)  4,130 replays/s
    30,000/30,000  (100.0%)  4,131 replays/s
  done in 7.3s

======================================================================
KELLY BANKROLL REPLAY  —  k = 7.000
======================================================================
Replays                                      30,000
Shoes per session                                15
Starting bankroll (B0)         $           5,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=7.000 | B0=$5,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                             15

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                175
TC +3                          $                350
TC +4                          $                525
TC +5                          $                650
TC +6                          $                875
TC +7                          $              1,050
TC +8                          $              1,225
TC +9                          $              1,375
TC +10                         $              1,500

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +644.66
P(finish down)  (end < B0)                   48.81%
P(ruin)                                      31.55%
P(end < 0.9·B0)                              45.91%
P(end < 0.75·B0)                             42.13%
P(end < 0.5·B0)                              36.75%
P(end < 0.25·B0)                             33.06%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           5,644.66
Median ending bankroll         $           5,200.00
95% range [p2.5 .. p97.5]      $-1,025.00  ..  $18,600.00
P(lose > 25%)                                42.13%
P(lose > 50%)                                36.75%
P(double bankroll)                           21.64%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           4,224.28
Median max drawdown            $           3,950.00
95th pct max drawdown          $           7,550.00
99th pct max drawdown          $           9,662.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0788%
Avg session wagered            $          59,755.28

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               12.7
Avg hands played                              541.4
======================================================================

Figure saved to: kelly_sweep_hilo_6d75pen_h17_br5000min25.png
