Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  90 TC buckets in [-23.0 .. +23.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 43,061,863 hands (avg 43.1 hands/shoe) in 9.3s

→ Kelly k=0.250   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,131 replays/s
     6,000/30,000  ( 20.0%)  5,122 replays/s
     9,000/30,000  ( 30.0%)  5,138 replays/s
    12,000/30,000  ( 40.0%)  5,145 replays/s
    15,000/30,000  ( 50.0%)  5,150 replays/s
    18,000/30,000  ( 60.0%)  5,154 replays/s
    21,000/30,000  ( 70.0%)  5,155 replays/s
    24,000/30,000  ( 80.0%)  5,158 replays/s
    27,000/30,000  ( 90.0%)  5,160 replays/s
    30,000/30,000  (100.0%)  5,157 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.250 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $             -39.04
P(finish down)  (end < B0)                   52.13%
P(ruin)                                       0.00%
P(end < 0.9·B0)                               7.34%
P(end < 0.75·B0)                              0.02%
P(end < 0.5·B0)                               0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $           9,960.96
Median ending bankroll         $           9,962.50
95% range [p2.5 .. p97.5]      $ 8,637.50  ..  $11,262.50
P(lose > 25%)                                 0.02%
P(lose > 50%)                                 0.00%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             442.15
Median max drawdown            $             412.50
95th pct max drawdown          $             700.00
99th pct max drawdown          $             912.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                        99.7013%
Avg session wagered            $          13,069.92

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=0.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,156 replays/s
     6,000/30,000  ( 20.0%)  5,156 replays/s
     9,000/30,000  ( 30.0%)  5,158 replays/s
    12,000/30,000  ( 40.0%)  5,156 replays/s
    15,000/30,000  ( 50.0%)  5,156 replays/s
    18,000/30,000  ( 60.0%)  5,156 replays/s
    21,000/30,000  ( 70.0%)  5,156 replays/s
    24,000/30,000  ( 80.0%)  5,154 replays/s
    27,000/30,000  ( 90.0%)  5,150 replays/s
    30,000/30,000  (100.0%)  5,148 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.500 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $              +7.53
P(finish down)  (end < B0)                   49.36%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              12.98%
P(end < 0.75·B0)                              0.53%
P(end < 0.5·B0)                               0.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,007.53
Median ending bankroll         $          10,012.50
95% range [p2.5 .. p97.5]      $ 8,175.00  ..  $11,850.00
P(lose > 25%)                                 0.53%
P(lose > 50%)                                 0.00%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             661.51
Median max drawdown            $             575.00
95th pct max drawdown          $           1,275.00
99th pct max drawdown          $           1,725.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.0482%
Avg session wagered            $          15,624.55

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=0.750   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,122 replays/s
     6,000/30,000  ( 20.0%)  5,159 replays/s
     9,000/30,000  ( 30.0%)  5,169 replays/s
    12,000/30,000  ( 40.0%)  5,170 replays/s
    15,000/30,000  ( 50.0%)  5,177 replays/s
    18,000/30,000  ( 60.0%)  5,183 replays/s
    21,000/30,000  ( 70.0%)  5,188 replays/s
    24,000/30,000  ( 80.0%)  5,190 replays/s
    27,000/30,000  ( 90.0%)  5,191 replays/s
    30,000/30,000  (100.0%)  5,189 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=0.750 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $             +53.06
P(finish down)  (end < B0)                   47.89%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              17.73%
P(end < 0.75·B0)                              2.08%
P(end < 0.5·B0)                               0.06%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,053.06
Median ending bankroll         $          10,050.00
95% range [p2.5 .. p97.5]      $ 7,600.00  ..  $12,537.50
P(lose > 25%)                                 2.08%
P(lose > 50%)                                 0.06%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $             907.11
Median max drawdown            $             787.50
95th pct max drawdown          $           1,850.00
99th pct max drawdown          $           2,500.12

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.2915%
Avg session wagered            $          18,204.86

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=1.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,125 replays/s
     6,000/30,000  ( 20.0%)  5,156 replays/s
     9,000/30,000  ( 30.0%)  5,161 replays/s
    12,000/30,000  ( 40.0%)  5,166 replays/s
    15,000/30,000  ( 50.0%)  5,172 replays/s
    18,000/30,000  ( 60.0%)  5,176 replays/s
    21,000/30,000  ( 70.0%)  5,179 replays/s
    24,000/30,000  ( 80.0%)  5,181 replays/s
    27,000/30,000  ( 90.0%)  5,183 replays/s
    30,000/30,000  (100.0%)  5,184 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.000 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $             +96.74
P(finish down)  (end < B0)                   47.14%
P(ruin)                                       0.00%
P(end < 0.9·B0)                              21.68%
P(end < 0.75·B0)                              4.50%
P(end < 0.5·B0)                               0.26%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,096.74
Median ending bankroll         $          10,087.50
95% range [p2.5 .. p97.5]      $ 6,975.00  ..  $13,262.50
P(lose > 25%)                                 4.50%
P(lose > 50%)                                 0.26%
P(double bankroll)                            0.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,169.64
Median max drawdown            $           1,025.00
95th pct max drawdown          $           2,425.00
99th pct max drawdown          $           3,300.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.4679%
Avg session wagered            $          20,678.01

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=1.250   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,160 replays/s
     6,000/30,000  ( 20.0%)  5,173 replays/s
     9,000/30,000  ( 30.0%)  5,178 replays/s
    12,000/30,000  ( 40.0%)  5,178 replays/s
    15,000/30,000  ( 50.0%)  5,177 replays/s
    18,000/30,000  ( 60.0%)  5,185 replays/s
    21,000/30,000  ( 70.0%)  5,189 replays/s
    24,000/30,000  ( 80.0%)  5,188 replays/s
    27,000/30,000  ( 90.0%)  5,191 replays/s
    30,000/30,000  (100.0%)  5,190 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.250
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.250 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +141.44
P(finish down)  (end < B0)                   46.58%
P(ruin)                                       0.02%
P(end < 0.9·B0)                              24.78%
P(end < 0.75·B0)                              7.23%
P(end < 0.5·B0)                               0.66%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,141.44
Median ending bankroll         $          10,125.00
95% range [p2.5 .. p97.5]      $ 6,362.50  ..  $14,025.00
P(lose > 25%)                                 7.23%
P(lose > 50%)                                 0.66%
P(double bankroll)                            0.01%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,435.88
Median max drawdown            $           1,250.00
95th pct max drawdown          $           3,000.00
99th pct max drawdown          $           4,087.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.6028%
Avg session wagered            $          23,462.47

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=1.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,161 replays/s
     6,000/30,000  ( 20.0%)  5,155 replays/s
     9,000/30,000  ( 30.0%)  5,151 replays/s
    12,000/30,000  ( 40.0%)  5,158 replays/s
    15,000/30,000  ( 50.0%)  5,165 replays/s
    18,000/30,000  ( 60.0%)  5,169 replays/s
    21,000/30,000  ( 70.0%)  5,171 replays/s
    24,000/30,000  ( 80.0%)  5,171 replays/s
    27,000/30,000  ( 90.0%)  5,173 replays/s
    30,000/30,000  (100.0%)  5,174 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=1.500 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +188.91
P(finish down)  (end < B0)                   46.10%
P(ruin)                                       0.05%
P(end < 0.9·B0)                              27.39%
P(end < 0.75·B0)                             10.08%
P(end < 0.5·B0)                               1.47%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,188.91
Median ending bankroll         $          10,175.00
95% range [p2.5 .. p97.5]      $ 5,637.50  ..  $14,825.00
P(lose > 25%)                                10.08%
P(lose > 50%)                                 1.47%
P(double bankroll)                            0.04%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           1,727.97
Median max drawdown            $           1,512.50
95th pct max drawdown          $           3,612.50
99th pct max drawdown          $           4,912.62

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.7074%
Avg session wagered            $          26,703.79

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.6
======================================================================

→ Kelly k=2.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,176 replays/s
     6,000/30,000  ( 20.0%)  5,182 replays/s
     9,000/30,000  ( 30.0%)  5,170 replays/s
    12,000/30,000  ( 40.0%)  5,177 replays/s
    15,000/30,000  ( 50.0%)  5,182 replays/s
    18,000/30,000  ( 60.0%)  5,176 replays/s
    21,000/30,000  ( 70.0%)  5,169 replays/s
    24,000/30,000  ( 80.0%)  5,167 replays/s
    27,000/30,000  ( 90.0%)  5,168 replays/s
    30,000/30,000  (100.0%)  5,164 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.000 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +280.44
P(finish down)  (end < B0)                   45.53%
P(ruin)                                       0.32%
P(end < 0.9·B0)                              30.68%
P(end < 0.75·B0)                             14.89%
P(end < 0.5·B0)                               3.74%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,280.44
Median ending bankroll         $          10,262.50
95% range [p2.5 .. p97.5]      $ 4,325.00  ..  $16,375.00
P(lose > 25%)                                14.89%
P(lose > 50%)                                 3.74%
P(double bankroll)                            0.28%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,270.14
Median max drawdown            $           1,975.00
95th pct max drawdown          $           4,787.50
99th pct max drawdown          $           6,487.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.8828%
Avg session wagered            $          31,765.08

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              430.3
======================================================================

→ Kelly k=2.500   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,188 replays/s
     6,000/30,000  ( 20.0%)  5,196 replays/s
     9,000/30,000  ( 30.0%)  5,200 replays/s
    12,000/30,000  ( 40.0%)  5,203 replays/s
    15,000/30,000  ( 50.0%)  5,203 replays/s
    18,000/30,000  ( 60.0%)  5,203 replays/s
    21,000/30,000  ( 70.0%)  5,205 replays/s
    24,000/30,000  ( 80.0%)  5,206 replays/s
    27,000/30,000  ( 90.0%)  5,207 replays/s
    30,000/30,000  (100.0%)  5,205 replays/s
  done in 5.8s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=2.500 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +371.09
P(finish down)  (end < B0)                   45.16%
P(ruin)                                       1.12%
P(end < 0.9·B0)                              33.23%
P(end < 0.75·B0)                             18.77%
P(end < 0.5·B0)                               6.61%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,371.09
Median ending bankroll         $          10,337.50
95% range [p2.5 .. p97.5]      $ 2,812.19  ..  $17,987.50
P(lose > 25%)                                18.77%
P(lose > 50%)                                 6.61%
P(double bankroll)                            0.99%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           2,840.57
Median max drawdown            $           2,487.50
95th pct max drawdown          $           5,975.00
99th pct max drawdown          $           8,075.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.9836%
Avg session wagered            $          37,728.91

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                               10.0
Avg hands played                              429.1
======================================================================

→ Kelly k=3.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,248 replays/s
     6,000/30,000  ( 20.0%)  5,221 replays/s
     9,000/30,000  ( 30.0%)  5,212 replays/s
    12,000/30,000  ( 40.0%)  5,221 replays/s
    15,000/30,000  ( 50.0%)  5,223 replays/s
    18,000/30,000  ( 60.0%)  5,222 replays/s
    21,000/30,000  ( 70.0%)  5,225 replays/s
    24,000/30,000  ( 80.0%)  5,228 replays/s
    27,000/30,000  ( 90.0%)  5,229 replays/s
    30,000/30,000  (100.0%)  5,231 replays/s
  done in 5.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=3.000 | B0=$10,000 | min=$25 / max=$2000

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
E[profit]  = E[end − B0]       $            +454.57
P(finish down)  (end < B0)                   44.95%
P(ruin)                                       2.49%
P(end < 0.9·B0)                              34.78%
P(end < 0.75·B0)                             21.89%
P(end < 0.5·B0)                               9.41%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,454.57
Median ending bankroll         $          10,425.00
95% range [p2.5 .. p97.5]      $    12.50  ..  $19,550.31
P(lose > 25%)                                21.89%
P(lose > 50%)                                 9.41%
P(double bankroll)                            2.11%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           3,377.53
Median max drawdown            $           2,962.50
95th pct max drawdown          $           7,112.50
99th pct max drawdown          $           9,500.12

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0695%
Avg session wagered            $          42,502.13

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.9
Avg hands played                              426.9
======================================================================

→ Kelly k=4.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,280 replays/s
     6,000/30,000  ( 20.0%)  5,273 replays/s
     9,000/30,000  ( 30.0%)  5,276 replays/s
    12,000/30,000  ( 40.0%)  5,285 replays/s
    15,000/30,000  ( 50.0%)  5,292 replays/s
    18,000/30,000  ( 60.0%)  5,295 replays/s
    21,000/30,000  ( 70.0%)  5,300 replays/s
    24,000/30,000  ( 80.0%)  5,306 replays/s
    27,000/30,000  ( 90.0%)  5,306 replays/s
    30,000/30,000  (100.0%)  5,308 replays/s
  done in 5.7s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=4.000 | B0=$10,000 | min=$25 / max=$2000

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
TC +2                          $                200
TC +3                          $                400
TC +4                          $                600
TC +5                          $                750
TC +6                          $              1,000
TC +7                          $              1,200
TC +8                          $              1,400
TC +9                          $              1,575
TC +10                         $              1,700

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +612.03
P(finish down)  (end < B0)                   44.96%
P(ruin)                                       6.88%
P(end < 0.9·B0)                              37.19%
P(end < 0.75·B0)                             26.82%
P(end < 0.5·B0)                              15.18%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,612.03
Median ending bankroll         $          10,587.50
95% range [p2.5 .. p97.5]      $  -575.00  ..  $22,725.31
P(lose > 25%)                                26.82%
P(lose > 50%)                                15.18%
P(double bankroll)                            5.41%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           4,460.47
Median max drawdown            $           3,962.50
95th pct max drawdown          $           9,225.00
99th pct max drawdown          $          11,825.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.1494%
Avg session wagered            $          53,249.65

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.8
Avg hands played                              419.3
======================================================================

→ Kelly k=5.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,479 replays/s
     6,000/30,000  ( 20.0%)  5,454 replays/s
     9,000/30,000  ( 30.0%)  5,446 replays/s
    12,000/30,000  ( 40.0%)  5,442 replays/s
    15,000/30,000  ( 50.0%)  5,439 replays/s
    18,000/30,000  ( 60.0%)  5,438 replays/s
    21,000/30,000  ( 70.0%)  5,440 replays/s
    24,000/30,000  ( 80.0%)  5,439 replays/s
    27,000/30,000  ( 90.0%)  5,440 replays/s
    30,000/30,000  (100.0%)  5,436 replays/s
  done in 5.5s

======================================================================
KELLY BANKROLL REPLAY  —  k = 5.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=5.000 | B0=$10,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                             22

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                250
TC +3                          $                500
TC +4                          $                750
TC +5                          $                950
TC +6                          $              1,275
TC +7                          $              1,500
TC +8                          $              1,750
TC +9                          $              1,975
TC +10                         $              2,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +744.11
P(finish down)  (end < B0)                   45.28%
P(ruin)                                      12.06%
P(end < 0.9·B0)                              39.07%
P(end < 0.75·B0)                             30.72%
P(end < 0.5·B0)                              20.24%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,744.11
Median ending bankroll         $          10,700.00
95% range [p2.5 .. p97.5]      $-1,037.50  ..  $25,787.81
P(lose > 25%)                                30.72%
P(lose > 50%)                                20.24%
P(double bankroll)                            9.21%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           5,455.29
Median max drawdown            $           4,937.50
95th pct max drawdown          $          10,875.00
99th pct max drawdown          $          13,762.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.2019%
Avg session wagered            $          61,911.24

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.5
Avg hands played                              409.1
======================================================================

→ Kelly k=6.000   (30,000 replays × 10 shoes)
     3,000/30,000  ( 10.0%)  5,548 replays/s
     6,000/30,000  ( 20.0%)  5,529 replays/s
     9,000/30,000  ( 30.0%)  5,531 replays/s
    12,000/30,000  ( 40.0%)  5,520 replays/s
    15,000/30,000  ( 50.0%)  5,529 replays/s
    18,000/30,000  ( 60.0%)  5,525 replays/s
    21,000/30,000  ( 70.0%)  5,525 replays/s
    24,000/30,000  ( 80.0%)  5,527 replays/s
    27,000/30,000  ( 90.0%)  5,538 replays/s
    30,000/30,000  (100.0%)  5,544 replays/s
  done in 5.4s

======================================================================
KELLY BANKROLL REPLAY  —  k = 6.000
======================================================================
Replays                                      30,000
Shoes per session                                10
Starting bankroll (B0)         $          10,000.00
Table min / max                 $25 / $2000
Ruin mode                                   anytime
Sizer                          Kelly k=6.000 | B0=$10,000 | min=$25 / max=$2000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $       25  /  $    2,000
Median bucket bet              $                 25
Spread (max / min)                           80.00×
Buckets at table min                             54
Buckets at table max                             26

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                 25
TC +2                          $                300
TC +3                          $                600
TC +4                          $                900
TC +5                          $              1,125
TC +6                          $              1,525
TC +7                          $              1,800
TC +8                          $              2,000
TC +9                          $              2,000
TC +10                         $              2,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $            +842.94
P(finish down)  (end < B0)                   46.03%
P(ruin)                                      16.93%
P(end < 0.9·B0)                              40.74%
P(end < 0.75·B0)                             33.85%
P(end < 0.5·B0)                              24.43%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $          10,842.94
Median ending bankroll         $          10,725.00
95% range [p2.5 .. p97.5]      $-1,350.00  ..  $28,525.00
P(lose > 25%)                                33.85%
P(lose > 50%)                                24.43%
P(double bankroll)                           12.53%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           6,305.91
Median max drawdown            $           5,825.00
95th pct max drawdown          $          12,037.50
99th pct max drawdown          $          15,187.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.2163%
Avg session wagered            $          69,301.43

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                                9.3
Avg hands played                              398.6
======================================================================

Figure saved to: kelly_sweep_hilo_6d75pen_h17.png
