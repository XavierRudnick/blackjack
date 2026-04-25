Loading EV table from: data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
  90 TC buckets in [-23.0 .. +23.5] (HandsPlayed >= 0)
Loading traces from: stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst
  Loaded 1,000,000 shoes | 43,061,863 hands (avg 43.1 hands/shoe) in 9.1s

→ Kelly k=0.250   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.250
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=0.250 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             56
Buckets at table max                              4

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                100
TC +3                          $                200
TC +4                          $                400
TC +5                          $                500
TC +6                          $                600
TC +7                          $                800
TC +8                          $                900
TC +9                          $              1,000
TC +10                         $              1,100

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $          +8,175.25
P(finish down)  (end < B0)                   42.56%
P(ruin)                                       0.58%
P(end < 0.9·B0)                              32.98%
P(end < 0.75·B0)                             20.96%
P(end < 0.5·B0)                               7.60%
P(end < 0.25·B0)                              2.26%
P(end < 0.1·B0)                               1.00%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         108,175.25
Median ending bankroll         $         107,600.00
95% range [p2.5 .. p97.5]      $26,198.75  ..  $188,712.50
P(lose > 25%)                                20.96%
P(lose > 50%)                                 7.60%
P(double bankroll)                            1.40%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $           9,397.76
Median max drawdown            $           9,100.00
95th pct max drawdown          $          12,600.00
99th pct max drawdown          $          15,000.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.1237%
Avg session wagered            $       6,608,135.78

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=0.500   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.500
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=0.500 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             55
Buckets at table max                              5

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                300
TC +3                          $                500
TC +4                          $                700
TC +5                          $                900
TC +6                          $              1,300
TC +7                          $              1,500
TC +8                          $              1,800
TC +9                          $              2,000
TC +10                         $              2,100

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $         +51,822.08
P(finish down)  (end < B0)                   24.12%
P(ruin)                                       2.08%
P(end < 0.9·B0)                              20.20%
P(end < 0.75·B0)                             14.50%
P(end < 0.5·B0)                               8.04%
P(end < 0.25·B0)                              4.48%
P(end < 0.1·B0)                               2.86%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         151,822.08
Median ending bankroll         $         151,600.00
95% range [p2.5 .. p97.5]      $ 6,788.75  ..  $300,005.00
P(lose > 25%)                                14.50%
P(lose > 50%)                                 8.04%
P(double bankroll)                           26.02%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          18,438.56
Median max drawdown            $          17,900.00
95th pct max drawdown          $          24,900.00
99th pct max drawdown          $          29,353.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.5517%
Avg session wagered            $       9,392,487.23

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=0.750   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 0.750
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=0.750 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             11

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                400
TC +3                          $                700
TC +4                          $              1,100
TC +5                          $              1,400
TC +6                          $              1,900
TC +7                          $              2,300
TC +8                          $              2,600
TC +9                          $              3,000
TC +10                         $              3,200

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $         +95,297.66
P(finish down)  (end < B0)                   18.50%
P(ruin)                                       3.74%
P(end < 0.9·B0)                              16.34%
P(end < 0.75·B0)                             12.90%
P(end < 0.5·B0)                               8.42%
P(end < 0.25·B0)                              5.76%
P(end < 0.1·B0)                               4.50%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         195,297.66
Median ending bankroll         $         195,100.00
95% range [p2.5 .. p97.5]      $-16,801.25  ..  $412,055.00
P(lose > 25%)                                12.90%
P(lose > 50%)                                 8.42%
P(double bankroll)                           48.00%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          27,617.12
Median max drawdown            $          26,800.00
95th pct max drawdown          $          37,452.50
99th pct max drawdown          $          43,950.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.7808%
Avg session wagered            $      12,204,394.40

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=1.000   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.000
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=1.000 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             15

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                500
TC +3                          $              1,000
TC +4                          $              1,500
TC +5                          $              1,900
TC +6                          $              2,500
TC +7                          $              3,000
TC +8                          $              3,500
TC +9                          $              3,900
TC +10                         $              4,300

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +140,091.88
P(finish down)  (end < B0)                   16.34%
P(ruin)                                       4.84%
P(end < 0.9·B0)                              14.50%
P(end < 0.75·B0)                             12.16%
P(end < 0.5·B0)                               8.70%
P(end < 0.25·B0)                              6.74%
P(end < 0.1·B0)                               5.44%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         240,091.88
Median ending bankroll         $         238,075.00
95% range [p2.5 .. p97.5]      $-43,401.25  ..  $528,250.00
P(lose > 25%)                                12.16%
P(lose > 50%)                                 8.70%
P(double bankroll)                           60.90%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          36,635.59
Median max drawdown            $          35,600.00
95th pct max drawdown          $          49,200.00
99th pct max drawdown          $          57,850.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       100.9233%
Avg session wagered            $      15,172,504.86

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=1.500   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 1.500
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=1.500 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             26

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $                800
TC +3                          $              1,500
TC +4                          $              2,200
TC +5                          $              2,800
TC +6                          $              3,800
TC +7                          $              4,500
TC +8                          $              5,000
TC +9                          $              5,000
TC +10                         $              5,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +218,280.83
P(finish down)  (end < B0)                   14.38%
P(ruin)                                       6.02%
P(end < 0.9·B0)                              13.44%
P(end < 0.75·B0)                             11.80%
P(end < 0.5·B0)                               9.08%
P(end < 0.25·B0)                              7.44%
P(end < 0.1·B0)                               6.48%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         318,280.83
Median ending bankroll         $         314,225.00
95% range [p2.5 .. p97.5]      $-88,806.25  ..  $731,855.00
P(lose > 25%)                                11.80%
P(lose > 50%)                                 9.08%
P(double bankroll)                           71.08%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          50,452.27
Median max drawdown            $          49,450.00
95th pct max drawdown          $          65,415.00
99th pct max drawdown          $          76,600.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0600%
Avg session wagered            $      20,592,678.94

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=2.000   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.000
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=2.000 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             31

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $              1,000
TC +3                          $              2,000
TC +4                          $              3,000
TC +5                          $              3,800
TC +6                          $              5,000
TC +7                          $              5,000
TC +8                          $              5,000
TC +9                          $              5,000
TC +10                         $              5,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +278,030.15
P(finish down)  (end < B0)                   14.08%
P(ruin)                                       7.04%
P(end < 0.9·B0)                              13.12%
P(end < 0.75·B0)                             11.90%
P(end < 0.5·B0)                              10.02%
P(end < 0.25·B0)                              8.44%
P(end < 0.1·B0)                               7.42%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         378,030.15
Median ending bankroll         $         376,325.00
95% range [p2.5 .. p97.5]      $-131,083.75  ..  $894,758.75
P(lose > 25%)                                11.90%
P(lose > 50%)                                10.02%
P(double bankroll)                           75.10%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          59,548.69
Median max drawdown            $          58,300.00
95th pct max drawdown          $          75,450.00
99th pct max drawdown          $          86,600.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0877%
Avg session wagered            $      25,560,403.64

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=2.500   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.0s

======================================================================
KELLY BANKROLL REPLAY  —  k = 2.500
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=2.500 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             32

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $              1,300
TC +3                          $              2,500
TC +4                          $              3,700
TC +5                          $              4,700
TC +6                          $              5,000
TC +7                          $              5,000
TC +8                          $              5,000
TC +9                          $              5,000
TC +10                         $              5,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +321,114.67
P(finish down)  (end < B0)                   14.04%
P(ruin)                                       8.02%
P(end < 0.9·B0)                              13.18%
P(end < 0.75·B0)                             12.22%
P(end < 0.5·B0)                              10.68%
P(end < 0.25·B0)                              9.30%
P(end < 0.1·B0)                               8.58%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         421,114.67
Median ending bankroll         $         418,700.00
95% range [p2.5 .. p97.5]      $-167,076.25  ..  $1,018,701.25
P(lose > 25%)                                12.22%
P(lose > 50%)                                10.68%
P(double bankroll)                           76.54%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          65,719.22
Median max drawdown            $          64,600.00
95th pct max drawdown          $          82,500.00
99th pct max drawdown          $          93,250.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0811%
Avg session wagered            $      29,701,364.55

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=3.000   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 88.1s

======================================================================
KELLY BANKROLL REPLAY  —  k = 3.000
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=3.000 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             34

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $              1,500
TC +3                          $              3,000
TC +4                          $              4,500
TC +5                          $              5,000
TC +6                          $              5,000
TC +7                          $              5,000
TC +8                          $              5,000
TC +9                          $              5,000
TC +10                         $              5,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +352,806.11
P(finish down)  (end < B0)                   14.48%
P(ruin)                                       8.70%
P(end < 0.9·B0)                              13.68%
P(end < 0.75·B0)                             12.74%
P(end < 0.5·B0)                              11.28%
P(end < 0.25·B0)                              9.78%
P(end < 0.1·B0)                               9.08%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         452,806.11
Median ending bankroll         $         450,500.00
95% range [p2.5 .. p97.5]      $-196,306.25  ..  $1,125,807.50
P(lose > 25%)                                12.74%
P(lose > 50%)                                11.28%
P(double bankroll)                           77.40%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          70,581.30
Median max drawdown            $          69,100.00
95th pct max drawdown          $          89,310.00
99th pct max drawdown          $          99,450.50

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0690%
Avg session wagered            $      33,002,969.51

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

→ Kelly k=4.000   (5,000 replays × 1000 shoes)
       500/5,000  ( 10.0%)  57 replays/s
     1,000/5,000  ( 20.0%)  57 replays/s
     1,500/5,000  ( 30.0%)  57 replays/s
     2,000/5,000  ( 40.0%)  57 replays/s
     2,500/5,000  ( 50.0%)  57 replays/s
     3,000/5,000  ( 60.0%)  57 replays/s
     3,500/5,000  ( 70.0%)  57 replays/s
     4,000/5,000  ( 80.0%)  57 replays/s
     4,500/5,000  ( 90.0%)  57 replays/s
     5,000/5,000  (100.0%)  57 replays/s
  done in 87.9s

======================================================================
KELLY BANKROLL REPLAY  —  k = 4.000
======================================================================
Replays                                       5,000
Shoes per session                             1,000
Starting bankroll (B0)         $         100,000.00
Table min / max                 $100 / $5000
Ruin mode                                       end
Sizer                          Kelly k=4.000 | B0=$100,000 | min=$100 / max=$5000

CALCULATED BET SPREAD (EV grid, per TC step, clipped)
----------------------------------------------------------------------
TC range (CSV grid)                -23.0 ..     +23.5
Bet min / max                  $      100  /  $    5,000
Median bucket bet              $                100
Spread (max / min)                           50.00×
Buckets at table min                             54
Buckets at table max                             36

CALCULATED BET BY TC (integer TC +1 .. +10)
----------------------------------------------------------------------
TC +1                          $                100
TC +2                          $              2,100
TC +3                          $              3,900
TC +4                          $              5,000
TC +5                          $              5,000
TC +6                          $              5,000
TC +7                          $              5,000
TC +8                          $              5,000
TC +9                          $              5,000
TC +10                         $              5,000

USER PRIORITIES
----------------------------------------------------------------------
E[profit]  = E[end − B0]       $        +398,542.19
P(finish down)  (end < B0)                   15.46%
P(ruin)                                       9.62%
P(end < 0.9·B0)                              15.00%
P(end < 0.75·B0)                             13.86%
P(end < 0.5·B0)                              12.28%
P(end < 0.25·B0)                             10.76%
P(end < 0.1·B0)                              10.20%

BANKROLL DISTRIBUTION
----------------------------------------------------------------------
E[ending bankroll]             $         498,542.19
Median ending bankroll         $         493,175.00
95% range [p2.5 .. p97.5]      $-246,672.50  ..  $1,272,175.00
P(lose > 25%)                                13.86%
P(lose > 50%)                                12.28%
P(double bankroll)                           77.72%

DRAWDOWN DISTRIBUTION
----------------------------------------------------------------------
Mean max drawdown              $          76,869.49
Median max drawdown            $          75,600.00
95th pct max drawdown          $          95,500.00
99th pct max drawdown          $         106,500.00

RETURN TO PLAYER
----------------------------------------------------------------------
RTP                                       101.0229%
Avg session wagered            $      38,960,965.94

SESSION STATS
----------------------------------------------------------------------
Avg shoes played                             1000.0
Avg hands played                            43062.5
======================================================================

Figure saved to: kelly_sweep_hilo_6d75pen_h17.png
