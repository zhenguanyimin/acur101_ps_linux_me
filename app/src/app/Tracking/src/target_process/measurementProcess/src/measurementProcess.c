
#include "../include/measurementProcess.h"
#include"../../dispatch/include/dispatch.h"
#include"../../tracking/include/tracking_int.h"
//#include"../../tracking/include/tracking.h"

// Buffer initialization

//new
sDots_Cohesion_vector gDots_Cohesion_vector[1] = { 0 };
sClusterDetect gClusterDet[1] = { 0 };
float K_Ele_TAS[8] = {-0.2778,-0.2404,-0.2298,-0.1734,-0.1428,-0.1198,-0.1112,-0.1018 };//TAS俯仰比幅K系数：FOV=±(0.5:0.5:4)
float Ratio_Thrd_Lift_Ele_TAS[8] = { -0.0442,-0.0892,-0.1461,-0.2305,-0.3319,-0.4750,-0.6013,-0.7382 };//TAS俯仰差和比阈值下限：FOV=±(0.5:0.5:4)
float Ratio_Thrd_Right_Ele_TAS[8] = { 0.0187,0.0562,0.0818,0.1722,0.2791,0.3993,0.4977,0.6332 };//TAS俯仰差和比阈值上限：FOV=±(0.5:0.5:4)
float Gain_IF[] = { 14.2323,14.2923,14.3523,14.4123,14.4723,14.5323,14.9602,15.4620,16.0246,16.6615,17.3593,18.0048,18.5489,19.0408,19.5146,19.9710,20.4094,20.8615,21.3520,21.8561,22.3321,22.7639,23.1676,23.5488,23.9102,24.2491,24.5634,24.8508,25.1134,25.3561,25.5757,25.7752,25.9688,26.1633,26.3519,26.5283,26.6890,26.8376,26.9784,27.1132,27.2403,27.3592,27.4660,27.5645,27.6653,27.7747,27.8863,27.9921,28.0895,28.1809,28.2654,28.3429,28.4135,28.4761,28.5284,28.5728,28.6154,28.6590,28.7009,28.7393,28.7698,28.7969,28.8315,28.8818,28.9396,28.9920,29.0334,29.0694,29.1043,29.1396,29.1738,29.2062,29.2343,29.2605,29.2917,29.3305,29.3741,29.4212,29.4785,29.5391,29.5818,29.5955,29.5911,29.5802,29.5560,29.5253,29.5193,29.5605,29.6265,29.6814,29.7076,29.7228,29.7434,29.7798,29.8216,29.8540,29.8670,29.8706,29.8793,29.9012,29.9281,29.9509,29.9594,29.9637,29.9850,30.0407,30.1134,30.1730,
30.2057,30.2253,30.2426,30.2598,30.2748,30.2918,30.3129,30.3361,30.3592,30.3808,30.4023,30.4254,30.4545,30.4851,30.5061,30.5053,30.4948,30.4997,30.5366,30.5890,30.6323,30.6564,30.6715,30.6833,30.6879,30.6892,30.7051,30.7501,30.8097,30.8587,30.8867,30.9041,30.9178,30.9267,30.9320,30.9441,30.9706,31.0038,31.0317,31.0463,31.0555,31.0714,31.0975,31.1303,31.1708,31.2268,31.2905,31.3393,31.3613,31.3683,31.3734,31.3797,31.3841,31.3910,31.4008,31.4132,31.4312,31.4586,31.4915,31.5219,31.5475,31.5706,31.5886,31.5973,31.6009,31.6108,31.6309,31.6572,31.6893,31.7309,31.7784,31.8208,31.8561,31.8862,31.9061,31.9076,31.8989,31.8989,31.9170,31.9439,31.9705,31.9910,32.0113,32.0400,32.0866,32.1416,32.1850,32.2065,32.2164,32.2264,32.2392,32.2521,32.2688,32.2936,32.3222,32.3463,32.3599,32.3691,32.3832,32.4089,32.4396,32.4650,32.4775,32.4846,32.4992,32.5273,32.5628,32.6001,
32.6391,32.6798,32.7163,32.7478,32.7751,32.7945,32.7995,32.7967,32.8021,32.8259,32.8578,32.8836,32.8967,32.9037,32.9095,32.9143,32.9179,32.9247,32.9326,32.9436,32.9678,33.0139,33.0732,33.1297,33.1805,33.2285,33.2668,33.2875,33.2984,33.3158,33.3467,33.3841,33.4224,33.4677,33.5139,33.5377,33.5198,33.4794,33.4506,33.4457,33.4523,33.4680,33.4946,33.5304,33.5671,33.6025,33.6387,33.6748,33.7141,33.7532,33.7810,33.7895,33.7868,33.7857,33.7895,33.7949,33.8042,33.8173,33.8342,33.8576,33.8916,33.9320,33.9690,33.9991,34.0258,34.0497,34.0703,34.0881,34.1057,34.1226,34.1392,34.1596,34.1859,34.2160,34.2472,34.2812,34.3164,34.3453,34.3592,34.3668,34.3866,34.4336,34.4928,34.5377,34.5539,34.5559,34.5603,34.5723,34.5869,34.6055,34.6310,34.6606,34.6876,34.7102,34.7302,34.7458,34.7523,34.7544,34.7646,34.7887,34.8209,34.8555,34.8911,34.9290,34.9681,35.0142,35.0613,35.0911,
35.0904,35.0723,35.0575,35.0543,35.0544,35.0538,35.0442,35.0338,35.0430,35.0876,35.1517,35.2079,35.2444,35.2731,35.3018,35.3373,35.3729,35.3970,35.4007,35.3930,35.3887,35.3909,35.3966,35.4122,35.4456,35.4889,35.5248,35.5439,35.5557,35.5709,35.5952,35.6229,35.6488,35.6707,35.6907,35.7100,35.7278,35.7450,35.7660,35.7812,35.8002,35.7860,35.8060,35.8260,35.8460,35.8660,35.8860,35.9060,35.9260,35.9460,35.9660,35.9564,35.9794,36.0085,36.0445,36.0866,36.1232,36.1483,36.1678,36.1880,36.2133,36.2393,36.2590,36.2675,36.2698,36.2738,36.2816,36.2912,36.3041,36.3229,36.3449,36.3637,36.3746,36.3822,36.3937,36.4112,36.4326,36.4589,36.4936,36.5333,36.5682,36.5942,36.6154,36.6343,36.6520,36.6674,36.6802,36.6857,36.6885,36.7027,36.7422,36.7932,36.8278,36.8292,36.8142,36.8062,36.8145,36.8299,36.8473,36.8636,36.8819,36.9069,36.9439,36.9875,37.0260,37.0546,37.0781,37.0986,
37.1164,37.1313,37.1451,37.1572,37.1682,37.1812,37.1998,37.2204,37.2360,37.2412,37.2414,37.2455,37.2563,37.2710,37.2906,37.3202,37.3546,37.3791,37.3829,37.3768,37.3787,37.3974,37.4241,37.4496,37.4700,37.4892,37.5093,37.5310,37.5536,37.5777,37.6066,37.6370,37.6587,37.6615,37.6556,37.6623,37.6938,37.7378,37.7784,37.8138,37.8458,37.8648,37.8586,37.8394,37.8335,37.8586,37.8970,37.9222,37.9161,37.8968,37.8918,37.9167,37.9559,37.9898,38.0108,38.0265,38.0401,38.0516,38.0610,38.0723,38.0856,38.1007,38.1207,38.1508,38.1859,38.2135,38.2255,38.2300,38.2384,38.2539,38.2734,38.2984,38.3331,38.3733,38.4077,38.4298,38.4461,38.4642,38.4900,38.5177,38.5371,38.5406,38.5359,38.5352,38.5443,38.5575,38.5701,38.5773,38.5840,38.5995,38.6325,38.6743,38.7085,38.7266,38.7370,38.7487,38.7678,38.7883,38.8007,38.7981,38.7875,38.7796,38.7759,38.7750,38.7832,38.8055,38.8370,38.8685,
38.9003,38.9322,38.9543,38.9576,38.9512,38.9514,38.9638,38.9829,39.0093,39.0489,39.0958,39.1326,39.1520,39.1613,39.1645,39.1572,39.1438,39.1418,39.1623,39.1940,39.2212,39.2383,39.2508,39.2591,39.2618,39.2602,39.2592,39.2542,39.2497,39.2631,39.3087,39.3721,39.4274,39.4666,39.4977,39.5186,39.5232,39.5176,39.5179,39.5317,39.5514,39.5703,39.5862,39.6012,39.6156,39.6303,39.6444,39.6557,39.6613,39.6642,39.6706,39.6822,39.6974,39.7178,39.7475,39.7824,39.8115,39.8276,39.8379,39.8532,39.8827,39.9173,39.9402,39.9391,39.9265,39.9225,39.9397,39.9657,39.9839,39.9824,39.9732,39.9756,40.0031,40.0423,40.0727,40.0820,40.0824,40.0902,40.1151,40.1473,40.1742,40.1914,40.2033,40.2097,40.2043,40.1934,40.1962,40.2270,40.2714,40.3057,40.3174,40.3189,40.3236,40.3361,40.3518,40.3696,40.3910,40.4146,40.4346,40.4485,40.4589,40.4681,40.4746,40.4800,40.4910,40.5124,40.5395,40.5648,
40.5844,40.6023,40.6232,40.6532,40.6863,40.7093,40.7142,40.7092,40.7050,40.7018,40.6995,40.7089,40.7411,40.7850,40.8184,40.8289,40.8288,40.8325,40.8466,40.8644,40.8803,40.8881,40.8938,40.9096,40.9457,40.9918,41.0294,41.0483,41.0587,41.0722,41.0984,41.1277,41.1429,41.1326,41.1081,41.0863,41.0691,41.0546,41.0528,41.0707,41.1014,41.1348,41.1674,41.2026,41.2405,41.2860,41.3342,41.3706,41.3866,41.3908,41.3940,41.4007,41.4066,41.4094,41.4054,41.3983,41.3975,41.4093,41.4274,41.4422,41.4467,41.4478,41.4570,41.4841,41.5191,41.5436,41.5426,41.5309,41.5346,41.5722,41.6251,41.6633,41.6722,41.6664,41.6593,41.6511,41.6417,41.6440,41.6654,41.6985,41.7340,41.7744,41.8172,41.8450,41.8456,41.8312,41.8220,41.8242,41.8315,41.8455,41.8683,41.8977,41.9286,41.9630,41.9989,42.0251,42.0370,42.0393,42.0343,42.0140,41.9865,41.9788,42.0105,42.0621,42.1015,42.1096,42.1056,42.1141,
42.1509,42.2003,42.2401,42.2625,42.2753,42.2795,42.2695,42.2509,42.2414,42.2476,42.2629,42.2854,42.3189,42.3596,42.3931,42.4159,42.4315,42.4365,42.4240,42.4008,42.3842,42.3787,42.3798,42.3904,42.4162,42.4516,42.4830,42.5028,42.5185,42.5389,42.5730,42.6118,42.6369,42.6365,42.6225,42.6125,42.6086,42.6087,42.6240,42.6704,42.7319,42.7721,42.7675,42.7417,42.7290,42.7447,42.7735,42.8041,42.8378,42.8733,42.8956,42.8914,42.8740,42.8684,42.8869,42.9172,42.9474,42.9736,42.9997,43.0244,43.0513,43.0769,43.0898,43.0792,43.0560,43.0414,43.0446,43.0565,43.0713,43.0869,43.1055,43.1274,43.1560,43.1878,43.2132,43.2260,43.2324,43.2407,43.2532,43.2677,43.2860,43.3111,43.3400,43.3661,43.3885,43.4082,43.4203,43.4188,43.4098,43.4077,43.4221,43.4434,43.4573,43.4531,43.4415,43.4395,43.4552,43.4804,43.5075,43.5375,43.5695,43.5924,43.5995,43.5975,43.5966,43.5983,43.6010,43.6104,
43.6328,43.6620,43.6847,43.6963,43.7015,43.7011,43.6899,43.6733,43.6680,43.6822,43.7079,43.7371,43.7675,43.8015,43.8380,43.8830,43.9305,43.9622,43.9652,43.9522,43.9429,43.9460,43.9526,43.9560,43.9500,43.9407,43.9399,43.9543,43.9772,43.9996,44.0201,44.0401,44.0551,44.0599,44.0596,44.0648,44.0818,44.1043,44.1238,44.1339,44.1410,44.1558,44.1844,44.2207,44.2567,44.2978,44.3387,44.3556,44.3260,44.2724,44.2390,44.2486,44.2783,44.3036,44.3134,44.3187,44.3283,44.3474,44.3709,44.3916,44.4068,44.4192,44.4295,44.4346,44.4377,44.4481,44.4727,44.5047,44.5333,44.5543,44.5719,44.5889,44.6087,44.6279,44.6386,44.6343,44.6215,44.6121,44.6077,44.6066,44.6165,44.6457,44.6858,44.7191,44.7372,44.7486,44.7608,44.7787,44.7974,44.8107,44.8143,44.8124,44.8119,44.8140,44.8176,44.8254,44.8383,44.8554,44.8770,44.9085,44.9445,44.9686,44.9684,44.9563,44.9538,44.9743,45.0044,45.0252,
45.0238,45.0132,45.0128,45.0355,45.0685,45.0924,45.0983,45.0951,45.0896,45.0804,45.0689,45.0659,45.0786,45.0998,45.1181,45.1295,45.1381,45.1450,45.1464,45.1462,45.1563,45.1856,45.2252,45.2615,45.2924,45.3200,45.3370,45.3315,45.3153,45.3167,45.3565,45.4138,45.4543,45.4608,45.4503,45.4402,45.4332,45.4265,45.4292,45.4491,45.4784,45.5028,45.5130,45.5184,45.5322,45.5656,45.6075,45.6381,45.6486,45.6478,45.6417,45.6263,45.6056,45.5981,45.6160,45.6470,45.6729,45.6864,45.6947,45.7008,45.7059,45.7087,45.7094,45.7025,45.6933,45.6981,45.7304,45.7766,45.8121,45.8247,45.8266,45.8296,45.8386,45.8487,45.8572,45.8625,45.8662,45.8703,45.8742,45.8785,45.8881,45.9067,45.9306,45.9533,45.9726,45.9907,46.0074,46.0234,46.0380,46.0494,46.0547,46.0569,46.0635,46.0799,46.1008,46.1176,46.1247,46.1277,46.1352,46.1541,46.1775,46.1929,46.1936,46.1863,46.1790,46.1745,46.1701,46.1657};

//float Gain_IF[] = { 0,23.4309, 23.4309, 23.4309, 23.4309, 23.4309, 23.4309, 23.4309, 23.4309, 23.4309, \
//23.4309, 23.4309, 23.4416, 23.4473, 23.4931, 23.5611, 23.6530, 23.7670, 23.8796, 23.9835, 24.0859, 24.1846, \
//24.2773, 24.3662, 24.4553, 24.5501, 24.6450, 24.7278, 24.7903, 24.8406, 24.8906, 24.9422, 24.9934, 25.0507, \
//25.1204, 25.1961, 25.2649, 25.3225, 25.3731, 25.4164, 25.4485, 25.4733, 25.5012, 25.5348, 25.5716, 25.6149, \
//25.6718, 25.7352, 25.7864, 25.8200, 25.8414, 25.8482, 25.8339, 25.8051, 25.7793, 25.7610, 25.7457, 25.7381,\
//25.7450, 25.7596, 25.7657, 25.7547, 25.7353, 25.7178, 25.7083, 25.7006, 25.6869, 25.6595, 25.6260, 25.6010, \
//25.5879, 25.5833, 25.5920, 25.6258, 25.6729, 25.7030, 25.7000, 25.6801, 25.6612, 25.6473, 25.6343, 25.6284, \
//25.6326, 25.6438, 25.6581, 25.6788, 25.7027, 25.7159, 25.7096, 25.6927, 25.6783, 25.6696, 25.6633, 25.6620, \
//25.6617, 25.6665, 25.6907, 25.7559, 25.8406, 25.8944, 25.8887, 25.8521, 25.8202, 25.8069, 25.7984, 25.7887, \
//25.7723, 25.7548, 25.7460, 25.7478, 25.7584, 25.7820, 25.8303, 25.8917, 25.9360, 25.9488, 25.9445, 25.9362, \
//25.9222, 25.9042, 25.9000, 25.9233, 25.9604, 25.9887, 25.9988, 26.0001, 25.9989, 25.9961, 25.9908, 25.9861, \
//25.9855, 25.9855, 25.9789, 25.9548, 25.9240, 25.9126, 25.9341, 25.9751, 26.0205, 26.0706, 26.1253, 26.1689, \
//26.1891, 26.1984, 26.2188, 26.2713, 26.3350, 26.3692, 26.3442, 26.2897, 26.2539, 26.2617, 26.2883, 26.3070, \
//26.3030, 26.2912, 26.2902, 26.3109, 26.3423, 26.3698, 26.3877, 26.4017, 26.4136, 26.4241, 26.4326, 26.4393, \
//26.4439, 26.4467, 26.4496, 26.4536, 26.4577, 26.4609, 26.4605, 26.4593, 26.4643, 26.4800, 26.5020, 26.5235, \
//26.5418, 26.5597, 26.5793, 26.6004, 26.6231, 26.6502, 26.6871, 26.7283, 26.7603, 26.7758, 26.7822, 26.7883, \
//26.7976, 26.8068, 26.8141, 26.8174, 26.8190, 26.8244, 26.8362, 26.8518, 26.8691, 26.8894, 26.9114, 26.9294, \
//26.9379, 26.9424, 26.9537, 26.9801, 27.0133, 27.0386, 27.0431, 27.0398, 27.0525, 27.1008, 27.1651, 27.2110, \
//27.2205, 27.2115, 27.2027, 27.1988, 27.1952, 27.1967, 27.2057, 27.2198, 27.2368, 27.2547, 27.2754, 27.3025, \
//27.3450, 27.3940, 27.4255, 27.4251, 27.4072, 27.3919, 27.3838, 27.3782, 27.3809, 27.3966, 27.4205, 27.4443, \
//27.4655, 27.4865, 27.5072, 27.5275, 27.5475, 27.5665, 27.5853, 27.6032, 27.6174, 27.6235, 27.6259, 27.6351, \
//27.6626, 27.6967, 27.7136, 27.6911, 27.6512, 27.6357, 27.6694, 27.7276, 27.7778, 27.8066, 27.8273, 27.8486, \
//27.8736, 27.8990, 27.9234, 27.9442, 27.9640, 27.9890, 28.0298, 28.0759, 28.1024, 28.0939, 28.0659, 28.0391, \
//28.0151, 27.9923, 27.9877, 28.0126, 28.0556, 28.0997, 28.1403, 28.1818, 28.2208, 28.2603, 28.2973, 28.3190, \
//28.3162, 28.2982, 28.2801, 28.2642, 28.2481, 28.2398, 28.2437, 28.2554, 28.2689, 28.2826, 28.2981, 28.3153, \
//28.3349, 28.3562, 28.3768, 28.3951, 28.4126, 28.4312, 28.4489, 28.4676, 28.4953, 28.5446, 28.6030, 28.6399, \
//28.6344, 28.6076, 28.5917, 28.6017, 28.6226, 28.6426, 28.6569, 28.6701, 28.6845, 28.7032, 28.7230, 28.7376, \
//28.7394, 28.7360, 28.7432, 28.7722, 28.8119, 28.8450, 28.8647, 28.8778, 28.8866, 28.8883, 28.8858, 28.8891, \
//28.8989, 28.9145, 28.9440, 28.9995, 29.0688, 29.1234, 29.1513, 29.1645, 29.1704, 29.1685, 29.1593, 29.1523, \
//29.1492, 29.1484, 29.1550, 29.1736, 29.1995, 29.2234, 29.2418, 29.2580, 29.2734, 29.2874, 29.3005, 29.3151, \
//29.3315, 29.3494, 29.3707, 29.4013, 29.4353, 29.4579, 29.4568, 29.4442, 29.4414, 29.4365, 29.4414, 29.5131, \
//29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, 29.5131, \
//29.5131, 29.6993, 29.7474, 29.7462, 29.7342, 29.7349, 29.7618, 29.8016, 29.8382, 29.8718, 29.9022, 29.9132, \
//29.8875, 29.8423, 29.8126, 29.8123, 29.8275, 29.8512, 29.8920, 29.9414, 29.9665, 29.9384, 29.8861, 29.8640, \
//29.9028, 29.9719, 30.0330, 30.0740, 30.1070, 30.1310, 30.1446, 30.1493, 30.1479, 30.1369, 30.1199, 30.1105, \
//30.1167, 30.1306, 30.1416, 30.1448, 30.1452, 30.1474, 30.1490, 30.1525, 30.1696, 30.2127, 30.2695, 30.3144, \
//30.3377, 30.3493, 30.3525, 30.3394, 30.3180, 30.3157, 30.3528, 30.4090, 30.4509, 30.4615, 30.4577, 30.4581, \
//30.4691, 30.4842, 30.5022, 30.5249, 30.5505, 30.5727, 30.5879, 30.5997, 30.6123, 30.6282, 30.6449, 30.6592, \
//30.6655, 30.6694, 30.6843, 30.7245, 30.7757, 30.8086, 30.8050, 30.7831, 30.7689, 30.7754, 30.7896, 30.7986, \
//30.7950, 30.7862, 30.7817, 30.7834, 30.7894, 30.8038, 30.8300, 30.8647, 30.9021, 30.9467, 30.9940, 31.0243, \
//31.0260, 31.0108, 30.9947, 30.9809, 30.9662, 30.9573, 30.9574, 30.9635, 30.9730, 30.9844, 30.9993, 31.0202, 
//31.0543, 31.0945, 31.1217, 31.1245, 31.1143, 31.1058, 31.1024, 31.1008, 31.1056, 31.1219, 31.1447, 31.1640, 
//31.1720, 31.1764, 31.1905, 31.2232, 31.2655, 31.3034, 31.3364, 31.3650, 31.3765, 31.3570, 31.3204, 31.2963, 
//31.2961, 31.3083, 31.3282, 31.3604, 31.4003, 31.4285, 31.4347, 31.4293, 31.4246, 31.4250, 31.4260, 31.4262, 
//31.4195, 31.4121, 31.4209, 31.4570, 31.5094, 31.5618, 31.6151, 31.6684, 31.7032, 31.7068, 31.6919, 31.6784, 
//31.6696, 31.6621, 31.6664, 31.6919, 31.7291, 31.7601, 31.7785, 31.7908, 31.7979, 31.8000, 31.7970, 31.7896, 
//31.7721, 31.7502, 31.7423, 31.7591, 31.7899, 31.8207, 31.8496, 31.8784, 31.8987, 31.9053, 31.9033, 31.8992, 
//31.8907, 31.8800, 31.8802, 31.9038, 31.9384, 31.9594, 31.9487, 31.9246, 31.9167, 31.9376, 31.9748, 32.0209, 
//32.0858, 32.1596, 32.2048, 32.1972, 32.1609, 32.1313, 32.1189, 32.1132, 32.1176, 32.1375, 32.1675, 32.1953, 
//32.2150, 32.2324, 32.2522, 32.2814, 32.3130, 32.3306, 32.3218, 32.2989, 32.2825, 32.2826, 32.2892, 32.2927, 
//32.2856, 32.2755, 32.2759, 32.2941, 32.3227, 32.3535, 32.3867, 32.4219, 32.4504, 32.4687, 32.4802, 32.4867, 
//32.4835, 32.4752, 32.4778, 32.5017, 32.5364, 32.5658, 32.5864, 32.6017, 32.6065, 32.5913, 32.5656, 32.5524, 
//32.5637, 32.5876, 32.6111, 32.6323, 32.6531, 32.6668, 32.6680, 32.6621, 32.6591, 32.6617, 32.6673, 32.6783, 
//32.6997, 32.7265, 32.7469, 32.7519, 32.7504, 32.7572, 32.7796, 32.8104, 32.8426, 32.8776, 32.9140, 32.9402, 
//32.9493, 32.9483, 32.9469, 32.9438, 32.9404, 32.9495, 32.9855, 33.0342, 33.0656, 33.0616, 33.0403, 33.0268, 
//33.0319, 33.0449, 33.0580, 33.0748, 33.0917, 33.0910, 33.0507, 32.9929, 32.9658, 33.0015, 33.0681, 33.1175, 
//33.1281, 33.1216, 33.1143, 33.1048, 33.0945, 33.1050, 33.1549, 33.2257, 33.2823, 33.3137, 33.3309, 33.3326, 
//33.3045, 33.2608, 33.2422, 33.2732, 33.3293, 33.3787, 33.4128, 33.4402, 33.4538, 33.4418, 33.4161, 33.4050, 
//33.4238, 33.4574, 33.4886, 33.5139, 33.5370, 33.5515, 33.5510, 33.5420, 33.5380, 33.5458, 33.5585, 33.5689, 
//33.5760, 33.5808, 33.5786, 33.5599, 33.5342, 33.5252, 33.5501, 33.5917, 33.6226, 33.6280, 33.6226, 33.6232, 
//33.6382, 33.6592, 33.6775, 33.6902, 33.7001, 33.7078, 33.7092, 33.7083, 33.7176, 33.7445, 33.7816, 33.8185, 
//33.8581, 33.8976, 33.9178, 33.9029, 33.8687, 33.8436, 33.8372, 33.8399, 33.8502, 33.8731, 33.9036, 33.9257, 
//33.9312, 33.9283, 33.9258, 33.9244, 33.9234, 33.9298, 33.9499, 33.9774, 34.0007, 34.0151, 34.0253, 34.0340, 
//34.0370, 34.0386, 34.0542, 34.1017, 34.1633, 34.2010, 34.1930, 34.1611, 34.1322, 34.1102, 34.0912, 34.0901, 
//34.1220, 34.1719, 34.2092, 34.2178, 34.2139, 34.2149, 34.2317, 34.2534, 34.2652, 34.2566, 34.2380, 34.2262, 
//34.2281, 34.2368, 34.2482, 34.2616, 34.2778, 34.2954, 34.3155, 34.3371, 34.3559, 34.3693, 34.3800, 34.3906, 
//34.4000, 34.4093, 34.4249, 34.4561, 34.4934, 34.5154, 34.5076, 34.4843, 34.4674, 34.4606, 34.4601, 34.4766, 
//34.5241, 34.5885, 34.6390, 34.6645, 34.6761, 34.6765, 34.6566, 34.6255, 34.6138, 34.6459, 34.6974, 34.7256, 
//34.7046, 34.6603, 34.6278, 34.6228, 34.6295, 34.6357, 34.6377, 34.6392, 34.6384, 34.6285, 34.6163, 34.6203, 
//34.6570, 34.7099, 34.7478, 34.7489, 34.7349, 34.7392, 34.7807, 34.8406, 34.8955, 34.9417, 34.9829, 35.0072, 
//35.0067, 34.9893, 34.9674, 34.9391, 34.9062, 34.8859, 34.8891, 34.9048, 34.9175, 34.9221, 34.9236, 34.9212, 
//34.9065, 34.8880, 34.8905, 34.9345, 34.9996, 35.0487, 35.0655, 35.0663, 35.0637, 35.0555, 35.0439, 35.0483, 
//35.0884, 35.1446, 35.1769, 35.1599, 35.1190, 35.0912, 35.0931, 35.1080, 35.1232, 35.1318, 35.1406, 35.1574, 
//35.1890, 35.2286, 35.2642, 35.2918, 35.3154, 35.3351, 35.3499, 35.3607, 35.3706, 35.3823, 35.3930, 35.3969, 
//35.3882, 35.3727, 35.3626, 35.3593, 35.3614, 35.3766, 35.4148, 35.4661, 35.5088, 35.5342, 35.5511, 35.5641, 
//35.5731, 35.5783, 35.5841, 35.5971, 35.6109, 35.6108, 35.5790, 35.5332, 35.5127, 35.5416, 35.5957, 35.6417, 
//35.6718, 35.6938, 35.6977, 35.6683, 35.6209, 35.5909, 35.5979, 35.6225, 35.6414, 35.6449, 35.6429, 35.6417, 
//35.6380, 35.6352, 35.6502, 35.6974, 35.7624, 35.8193, 35.8647, 35.9019, 35.9156, 35.8881, 35.8372, 35.7998, 
//35.7913, 35.7964, 35.8069, 35.8220, 35.8425, 35.8632, 35.8849, 35.9068, 35.9218, 35.9276, 35.9266, 35.9184, 
//35.8949, 35.8642, 35.8502, 35.8692, 35.9048, 35.9320, 35.9362, 35.9319, 35.9370, 35.9609, 35.9942, 36.0266, 
//36.0610, 36.0946, 36.1090, 36.0876, 36.0471, 36.0188, 36.0155, 36.0243, 36.0385, 36.0566, 36.0800, 36.1060, 
//36.1384, 36.1733, 36.1971, 36.2005, 36.1927, 36.1884, 36.1940, 36.2031, 36.2109, 36.2150, 36.2177, 36.2207, 
//36.2220, 36.2236, 36.2333, 36.2594, 36.2936, 36.3188, 36.3245, 36.3213, 36.3237, 36.3431, 36.3682, 36.3802, 
//36.3659, 36.3386, 36.3188, 36.3144, 36.3175, 36.3259, 36.3417, 36.3626, 36.3800, 36.3886, 36.3937, 36.4022, 
//36.4189, 36.4390, 36.4551, 36.4663, 36.4736, 36.4724, 36.4539, 36.4270, 36.4137, 36.4273, 36.4545, 36.4781, 
//36.4922, 36.5026, 36.5092, 36.5084, 36.5038, 36.5066, 36.5230, 36.5468, 36.5703, 36.5939, 36.6171, 36.6313, 
//36.6289, 36.6175, 36.6104, 36.6117, 36.6174, 36.6290, 36.6511, 36.6793, 36.7015, 36.7100, 36.7126, 36.7212, 
//36.7464, 36.7776, 36.7942, 36.7817, 36.7546, 36.7365, 36.7351, 36.7427, 36.7592 };

// (1) for 2nd generation(2#、3# radar)
float rangeThershold_gen2[CLUTTER_PRO_CASE] = { 51.0f,210.0f,306.0f,405.0f,633.0f,723.0f,1110.0f,2000.0f,2500.0f,3000.0f };//0423 test
float magThershold_dummy_gen2[CLUTTER_PRO_CASE + 1] = { 0.f };
float magThershold_start_gen2[CLUTTER_PRO_CASE + 1] = { 38.0f,34.0f,32.0f,32.0f,33.0f,35.0f,35.0f,1.0f,1.0f,1.0f,1.0f };//0423 test
float magThershold_end_gen2[CLUTTER_PRO_CASE + 1] = { 90.0f,90.0f,60.0f,54.0f,53.0f,48.0f,48.0f,1.0f,1.0f,1.0f,1.0f };//0423 test

//(2) for 3rd generation radar (sum and difference channel)
float rangeThershold_gen3[CLUTTER_PRO_CASE] = { 100.0f,210.0f,306.0f,405.0f,633.0f,723.0f,1110.0f,2000.0f,2500.0f,3000.0f };//0612 test
float magThershold_dummy_gen3[CLUTTER_PRO_CASE + 1] = { 0.f };
float magThershold_start_gen3[CLUTTER_PRO_CASE + 1] = { 25.0f,24.0f,24.0f,23.0f,23.0f,23.0f,23.0f,1.0f,1.0f,1.0f,1.0f };//0711 test
//float magThershold_start_gen3[CLUTTER_PRO_CASE + 1] = { 38.0f,38.0f,38.0f,37.0f,37.0f,37.0f,37.0f,1.0f,1.0f,1.0f,1.0f };//0612 test
//float magThershold_start_gen3[CLUTTER_PRO_CASE + 1] = { 27.0f,27.0f,27.0f,26.0f,26.0f,26.0f,24.0f,1.0f,1.0f,1.0f,1.0f };//0710 test
float magThershold_end_gen3[CLUTTER_PRO_CASE + 1] = { 100.0f,90.0f,80.0f,75.0f,74.0f,72.0f,72.5f,1.0f,1.0f,1.0f,1.0f };//0612 test

//// (2) for 2nd generation(4# radar) (TR undraw)
//float rangeThershold_gen3[CLUTTER_PRO_CASE] = { 100.0f,210.0f,306.0f,405.0f,633.0f,723.0f,1110.0f,2000.0f,2500.0f,3000.0f };//0612 test
//float magThershold_dummy_gen3[CLUTTER_PRO_CASE + 1] = { 0.f };
//float magThershold_start_gen3[CLUTTER_PRO_CASE + 1] = { 38.0f,38.0f,38.0f,40.0f,40.0f,40.0f,41.0f,1.0f,1.0f,1.0f,1.0f };//0612 test
//float magThershold_end_gen3[CLUTTER_PRO_CASE + 1] = { 100.0f,90.0f,80.0f,75.0f,74.0f,72.0f,72.5f,1.0f,1.0f,1.0f,1.0f };//0612 test

void measurement_process(sAlgObjData* algObj)
{
	sDetectFrame* detInst = (sDetectFrame*)algObj->detectsListOutput;//输入的波位检测信息
	sDispatchInst* dispatchInst = (sDispatchInst*)algObj->dispatch_inst;//输入的波位调度信息
	sDispatchOutput* dispatchOutput = dispatchInst->dispatchOutput;
	sMeasProcessInst* measInst = (sMeasProcessInst*)algObj->measurementProcess_inst;//波位点迹信息存储区   
	sMeasInput* measInput = (sMeasInput*)measInst->measInput;
	sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);//跟踪信息
	sTrackingFrame* trajInfoOutput = (sTrackingFrame*)tracking_inst->trajInfoOutput;
	sMeasOutput* measOutput = (sMeasOutput*)measInst->measOutput;//cohesived targets output to tracking
	sDotsCohesionFrame* dotsCoheOutput = (sDotsCohesionFrame*)measInst->dotsCoheOutput;//cohesived targets report
	sClusterDetect* clusterDet = gClusterDet;//波位内聚类点迹缓存结构体

	sMeasProcessInst* measProc_inst = (sMeasProcessInst*)algObj->measurementProcess_inst;
	sMeasConfigParam* measConfigParam = &measProc_inst->measConfigParam;

	//** 1.loading detection data  **//
	loadDetectionData(detInst, dispatchOutput, trajInfoOutput, measInput, tracking_inst->handle);

	//** 2.clutter process **//
	clutterProcess(measInput);//点迹数不变，加属性标记0,1,...

	//** 3.dots clustering, distinguish TWS/TAS mode**//
	dotsClustering(measInput, measInst, clusterDet);

	//** 4.judge whether target scan is finished,dots cohesion and output, distinguish TWS/TAS mode **//
	dotsCohesionAndOutput(measInput, measInst, measConfigParam);

	//** 5.Target Cluster Store Update, distinguish TWS/TAS mode **//
	tarClusterStoreUpdate(measInput, measInst, clusterDet);

	//** 6.output the dots cohesion , distinguish TWS/TAS mode **//
	dotsCohesion_unitReport(measInst);

}


//***************************************  Subfunction area  **************************************//

//** 1.loading detection data  **//
void loadDetectionData(sDetectFrame* detInst, sDispatchOutput* dispatchOutput, sTrackingFrame* trajInfoOutput,
	sMeasInput* measInput, void *handle)
{
	uint8_t i_det, iTarget;
	uint16_t tasObjId;	// index of tas object, only valid for tas scan mode
	float aziBeam = asinf((float)(dispatchOutput->aziBeamSin) * INV_ONE15FORMAT_SCALE) * RAD2DEG;
	float eleBeam = asinf((float)(dispatchOutput->eleBeamSin) * INV_ONE15FORMAT_SCALE) * RAD2DEG;
	sTracking_moduleInstance *tracking_inst = (sTracking_moduleInstance *)handle;
	/* 点迹输入结构体初始化 */
	memset(measInput, 0, sizeof(sMeasInput));

	//当前帧状态参数更新
	measInput->scanType = dispatchOutput->trackTwsTasFlag;
	measInput->curAziBeamRad = (int16_t)(asinf(dispatchOutput->aziBeamSin * INV_ONE15FORMAT_SCALE) * ONE13FORMAT_SCALE);
	measInput->wholeSpaceScanCycleFinalBeamFlag = dispatchOutput->wholeSpaceScanCycleFinalBeamFlag;
	measInput->tasOrInitTrackFinalBeamFlag = dispatchOutput->tasOrInitTrackFinalBeamFlag;
	measInput->tasAndInitTrackFinishBeamFlag = dispatchOutput->tasAndInitTrackFinishBeamFlag;
	measInput->frameID = detInst->frameID;
	measInput->detNum = detInst->detectObjNum;
	measInput->timestamp = (float)(detInst->timestamp) / 1000.f;//ms to s

	//当前帧点迹结构体更新
	if (detInst->detectObjNum > 0)
	{
		for (i_det = 0; i_det < detInst->detectObjNum; i_det++)
		{
			measInput->dotInput[i_det].dotId = detInst->detPointsList[i_det].id;
			measInput->dotInput[i_det].range = detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE;
			measInput->dotInput[i_det].doppler = detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE;
			measInput->dotInput[i_det].dopplerChn = detInst->detPointsList[i_det].dopplerChn;
			measInput->dotInput[i_det].azimuthRad = (float)(detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD);
			measInput->dotInput[i_det].elevationRad = (float)(detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD);
			measInput->dotInput[i_det].mag = detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE;
			measInput->dotInput[i_det].aziBeamRad = (int16_t)(asinf(dispatchOutput->aziBeamSin * INV_ONE15FORMAT_SCALE) * ONE13FORMAT_SCALE);
			measInput->dotInput[i_det].eleBeamRad = (int16_t)(asinf(dispatchOutput->eleBeamSin * INV_ONE15FORMAT_SCALE) * ONE13FORMAT_SCALE);
			//measInput->dotInput[i_det].aziBeamRad = (int16_t)(detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD * ONE13FORMAT_SCALE);
			//measInput->dotInput[i_det].eleBeamRad = (int16_t)(detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD * ONE13FORMAT_SCALE);
			measInput->dotInput[i_det].waveType = detInst->waveType;
			measInput->dotInput[i_det].dotCharacteristic = (uint8_t)(detInst->detPointsList[i_det].detProperty.classifyProb * INV_ONE6FORMAT_SCALE);
		}
	}

	//*** 读取TAS跟踪目标信息 ***//
	if (dispatchOutput->trackTwsTasFlag == TAS_SCAN)
	{
		tasObjId = dispatchOutput->tasObjId;
		for (iTarget = 0; iTarget < trajInfoOutput->trackObjNum; iTarget++)
		{
			if (tracking_inst->uidTrack[iTarget] == tasObjId)
			{
				measInput->tasTrackStr[0].tasTargetId = tasObjId;
				measInput->tasTrackStr[0].stateType = trajInfoOutput->trajList[iTarget].stateType;
				measInput->tasTrackStr[0].range = trajInfoOutput->trajList[iTarget].range * INV_ONE6FORMAT_SCALE;
				measInput->tasTrackStr[0].doppler = trajInfoOutput->trajList[iTarget].velocity * INV_ONE6FORMAT_SCALE;
				measInput->tasTrackStr[0].aziRad = trajInfoOutput->trajList[iTarget].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD;
				measInput->tasTrackStr[0].eleRad = trajInfoOutput->trajList[iTarget].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD;
				measInput->tasTrackStr[0].mag = trajInfoOutput->trajList[iTarget].mag * INV_ONE6FORMAT_SCALE;
				measInput->tasTrackStr[0].waveType = detInst->waveType;
				break;
			}
		}

		//*** 打印输出信息 ***//
#ifdef DEBUG_LOG_HXJ
		my_printf("dispatchInfo: wholeScanNum %d, azi pitch [%.2f %.2f],tasId %d  %d/%d\n", \
			dispatchOutput->wholeSpaceScanCycleCnt, aziBeam, eleBeam, dispatchOutput->tasObjId, \
			dispatchOutput->tasBeamCntCur, dispatchOutput->tasBeamTotal);

		my_printf("000011111-TAS-TrackStr-000011111 frameID[ %d ], curAziBeamRad[ %d ], tasTarId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ]\n", \
			measInput->frameID, measInput->curAziBeamRad, measInput->tasTrackStr[0].tasTargetId, measInput->tasTrackStr[0].range, \
			measInput->tasTrackStr[0].doppler, measInput->tasTrackStr[0].aziRad, measInput->tasTrackStr[0].eleRad, measInput->tasTrackStr[0].mag);
#endif // DEBUG_LOG_HXJ

	}

	//*** 打印输出信息 ***//
	if (detInst->detectObjNum > 0)
	{
		if (measInput->scanType == TWS_SCAN)
		{
#ifdef DEBUG_LOG_CFMSCAN
			fprintf(fp1, "\nTWS_SCAN	%u	%u	tasId:%u	finalbeam: %u	wavetype: %u	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", \
				detInst->frameID, g_scanType, measInput->tasTrackStr[0].tasTargetId, dispatchOutput->tasOrInitTrackFinalBeamFlag, dispatchOutput->waveType);
#endif
			for (i_det = 0; i_det < detInst->detectObjNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ
				my_printf("000000000-TWS-det-000000000 frameID[ %d ], curAziBeamRad[ %d ], detNum[ %d ], detId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],rSNR[ %d ], dSNR[ %d ]\n", \
					detInst->frameID, measInput->curAziBeamRad, detInst->detectObjNum, detInst->detPointsList[i_det].id,
					detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
					detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].detProperty.classification, detInst->detPointsList[i_det].detProperty.cohesionOkFlag);
#endif // DEBUG_LOG_HXJ

				/*fprintf(fp1, "det TWS:	%u	%u	%.3f	%u	%u	%.5f	%.5f	%.5f	%.5f	%.5f	%u	%u\n",\
					detInst->frameID, g_scanType,measInput->curAziBeamRad*INV_ONE13FORMAT_SCALE*RAD2DEG, detInst->detectObjNum, detInst->detPointsList[i_det].id,
					detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
					detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].detProperty.classification, detInst->detPointsList[i_det].detProperty.cohesionOkFlag);*/
#ifdef DEBUG_LOG_CFMSCAN
				fprintf(fp1, "det TWS:	frameid:%u	scantype:%u	cohesionOkFlag:%u	%.3f	%.3f	%.3f	%.3f\n", \
					detInst->frameID, g_scanType, detInst->detPointsList[i_det].detProperty.cohesionOkFlag, detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD);
#endif
			}
		}
		else
		{
#ifdef DEBUG_LOG_CFMSCAN
			fprintf(fp1, "\nTAS_SCAN	%u	%u	tasId:%u	finalbeam: %u	wavetype: %u	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", \
				detInst->frameID, g_scanType, measInput->tasTrackStr[0].tasTargetId, dispatchOutput->tasOrInitTrackFinalBeamFlag, dispatchOutput->waveType);
#endif
			for (i_det = 0; i_det < detInst->detectObjNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ
				my_printf("000000000-TAS-det-000000000 frameID[ %d ], curAziBeamRad[ %d ], detNum[ %d ], detId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],rSNR[ %d ], dSNR[ %d ]\n", \
					detInst->frameID, measInput->curAziBeamRad, detInst->detectObjNum, detInst->detPointsList[i_det].id,
					detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
					detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].detProperty.classification, detInst->detPointsList[i_det].detProperty.cohesionOkFlag);
#endif // DEBUG_LOG_HXJ
				/*fprintf(fp1, "det TAS:	%u	%u	%.3f	%u	%u	%.5f	%.5f	%.5f	%.5f	%.5f	%u	%u\n", \
					detInst->frameID, g_scanType, measInput->curAziBeamRad * INV_ONE13FORMAT_SCALE * RAD2DEG, detInst->detectObjNum, detInst->detPointsList[i_det].id,
					detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
					detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].detProperty.classification, detInst->detPointsList[i_det].detProperty.cohesionOkFlag);*/
#ifdef DEBUG_LOG_CFMSCAN
				fprintf(fp1, "det TWS:	frameid:%u	scantype:%u	cohesionOkFlag:%u	%.3f	%.3f	%.3f	%.3f\n", \
					detInst->frameID, g_scanType, detInst->detPointsList[i_det].detProperty.cohesionOkFlag, detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE, \
					detInst->detPointsList[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD);
#endif
			}
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ
		my_printf("000000001-det-000000001 frameID[ %d ], detNum[ %d ], curAziBeamRad[ %d ], \n", detInst->frameID, detInst->detectObjNum, (int16_t)(asinf(dispatchOutput->aziBeamSin * INV_ONE15FORMAT_SCALE) * ONE13FORMAT_SCALE));
#endif // DEBUG_LOG_HXJ
#ifdef DEBUG_LOG_CFMSCAN
		fprintf(fp1, "\nNoDetection	%u	scan_type:%u	%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n", detInst->frameID, g_scanType);
#endif
	}

#ifdef DEBUG_LOG_CFMSCAN
	fclose(fp1);
#endif
}



//** 2.clutter process**//
void clutterProcess(sMeasInput* measInput)
{
	uint8_t i_det;
	uint8_t detNum;
	float range, mag;
	float mag_thershold, mag_thershold_start, mag_thershold_end;
	float doppler = 0.f;
	float azimuth = 0.f;
	uint8_t isMirrorFreqPoint[MAX_NUM_DETECTS] = { 0 };
	sDots_Cohesion_vector* dotInput = measInput->dotInput;

	detNum = measInput->detNum;

	if (detNum > 0)
	{
		for (i_det = 0; i_det < detNum; i_det++)
		{
			if (0U == dotInput[i_det].dotCharacteristic)
			{
				dotInput[i_det].dotCharacteristic = REAL_POINT;
			}
			//dotInput[i_det].dotCharacteristic = REAL_POINT;
		}

		//**** (2.1)镜频抑制模块：速度维上中心对称且幅度相差较大认为是镜频（IQ通道幅度不平衡，相位不正交等原因导致） ****//
		ifMirrorFreqPoint(detNum, dotInput, isMirrorFreqPoint);

		for (i_det = 0; i_det < detNum; i_det++)
		{
			range = measInput->dotInput[i_det].range;
			mag = measInput->dotInput[i_det].mag;
			doppler = measInput->dotInput[i_det].doppler;
			azimuth = measInput->dotInput[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE * RAD2DEG;

			if (g_byMagClutRemove)
			{
				if (2 == g_byDevHwGen)
				{
					if (g_byDummyTest)
					{
						mag_thershold = ((range <= rangeThershold_gen2[0]) ? magThershold_dummy_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_dummy_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_dummy_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_dummy_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_dummy_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_dummy_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_dummy_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_dummy_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_dummy_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_dummy_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_dummy_gen2[10] : magThershold_dummy_gen2[11])))))))))));
					}
					else
					{
						mag_thershold_start = ((range <= rangeThershold_gen2[0]) ? magThershold_start_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_start_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_start_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_start_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_start_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_start_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_start_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_start_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_start_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_start_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_start_gen2[10] : magThershold_start_gen2[11])))))))))));
						mag_thershold_end = ((range <= rangeThershold_gen2[0]) ? magThershold_end_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_end_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_end_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_end_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_end_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_end_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_end_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_end_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_end_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_end_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_end_gen2[10] : magThershold_end_gen2[11])))))))))));
					}
				}
				else if (3 == g_byDevHwGen)
				{
					if (g_byDummyTest)
					{
						mag_thershold = ((range <= rangeThershold_gen3[0]) ? magThershold_dummy_gen3[0] : ((range <= rangeThershold_gen3[1]) ?
							magThershold_dummy_gen3[1] : ((range <= rangeThershold_gen3[2]) ? magThershold_dummy_gen3[2] : ((range <= rangeThershold_gen3[3]) ?
								magThershold_dummy_gen3[3] : ((range <= rangeThershold_gen3[4]) ? magThershold_dummy_gen3[4] : ((range <= rangeThershold_gen3[5]) ?
									magThershold_dummy_gen3[5] : ((range <= rangeThershold_gen3[6]) ? magThershold_dummy_gen3[6] : ((range <= rangeThershold_gen3[7]) ?
										magThershold_dummy_gen3[7] : ((range <= rangeThershold_gen3[8]) ? magThershold_dummy_gen3[8] : ((range <= rangeThershold_gen3[9]) ?
											magThershold_dummy_gen3[9] : ((range <= rangeThershold_gen3[10]) ? magThershold_dummy_gen3[10] : magThershold_dummy_gen3[11])))))))))));
					}
					else
					{
						mag_thershold_start = ((range <= rangeThershold_gen3[0]) ? magThershold_start_gen3[0] : ((range <= rangeThershold_gen3[1]) ?
							magThershold_start_gen3[1] : ((range <= rangeThershold_gen3[2]) ? magThershold_start_gen3[2] : ((range <= rangeThershold_gen3[3]) ?
								magThershold_start_gen3[3] : ((range <= rangeThershold_gen3[4]) ? magThershold_start_gen3[4] : ((range <= rangeThershold_gen3[5]) ?
									magThershold_start_gen3[5] : ((range <= rangeThershold_gen3[6]) ? magThershold_start_gen3[6] : ((range <= rangeThershold_gen3[7]) ?
										magThershold_start_gen3[7] : ((range <= rangeThershold_gen3[8]) ? magThershold_start_gen3[8] : ((range <= rangeThershold_gen3[9]) ?
											magThershold_start_gen3[9] : ((range <= rangeThershold_gen3[10]) ? magThershold_start_gen3[10] : magThershold_start_gen3[11])))))))))));
						mag_thershold_end = ((range <= rangeThershold_gen3[0]) ? magThershold_end_gen3[0] : ((range <= rangeThershold_gen3[1]) ?
							magThershold_end_gen3[1] : ((range <= rangeThershold_gen3[2]) ? magThershold_end_gen3[2] : ((range <= rangeThershold_gen3[3]) ?
								magThershold_end_gen3[3] : ((range <= rangeThershold_gen3[4]) ? magThershold_end_gen3[4] : ((range <= rangeThershold_gen3[5]) ?
									magThershold_end_gen3[5] : ((range <= rangeThershold_gen3[6]) ? magThershold_end_gen3[6] : ((range <= rangeThershold_gen3[7]) ?
										magThershold_end_gen3[7] : ((range <= rangeThershold_gen3[8]) ? magThershold_end_gen3[8] : ((range <= rangeThershold_gen3[9]) ?
											magThershold_end_gen3[9] : ((range <= rangeThershold_gen3[10]) ? magThershold_end_gen3[10] : magThershold_end_gen3[11])))))))))));
					}
				}
				else
				{// temp use 2nd generation thre
					if (g_byDummyTest)
					{
						mag_thershold = ((range <= rangeThershold_gen2[0]) ? magThershold_dummy_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_dummy_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_dummy_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_dummy_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_dummy_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_dummy_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_dummy_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_dummy_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_dummy_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_dummy_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_dummy_gen2[10] : magThershold_dummy_gen2[11])))))))))));
					}
					else
					{
						mag_thershold_start = ((range <= rangeThershold_gen2[0]) ? magThershold_start_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_start_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_start_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_start_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_start_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_start_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_start_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_start_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_start_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_start_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_start_gen2[10] : magThershold_start_gen2[11])))))))))));
						mag_thershold_end = ((range <= rangeThershold_gen2[0]) ? magThershold_end_gen2[0] : ((range <= rangeThershold_gen2[1]) ?
							magThershold_end_gen2[1] : ((range <= rangeThershold_gen2[2]) ? magThershold_end_gen2[2] : ((range <= rangeThershold_gen2[3]) ?
								magThershold_end_gen2[3] : ((range <= rangeThershold_gen2[4]) ? magThershold_end_gen2[4] : ((range <= rangeThershold_gen2[5]) ?
									magThershold_end_gen2[5] : ((range <= rangeThershold_gen2[6]) ? magThershold_end_gen2[6] : ((range <= rangeThershold_gen2[7]) ?
										magThershold_end_gen2[7] : ((range <= rangeThershold_gen2[8]) ? magThershold_end_gen2[8] : ((range <= rangeThershold_gen2[9]) ?
											magThershold_end_gen2[9] : ((range <= rangeThershold_gen2[10]) ? magThershold_end_gen2[10] : magThershold_end_gen2[11])))))))))));
					}
				}
			}


#ifdef DEMO_ACUR
			//**** (2.1)镜频点判断 ****//
			if (isMirrorFreqPoint[i_det] == 1)
			{
#ifdef DEBUG_LOG_ZQ
				my_printf("mirror freq point %d", dotInput[i_det].dotId);
#endif // DEBUG_ZQ
				dotInput[i_det].dotCharacteristic = MIRROR_FREQUENCE_POINT;
			}

			//**** (2.2)杂波点判断 ****//
			if (g_byMagClutRemove)
			{
				if (g_byDummyTest)
				{
					if (mag < mag_thershold)
					{
						dotInput[i_det].dotCharacteristic = CLUTTER_POINT;
					}
				}
				else
				{
					if ((mag < mag_thershold_start) || (mag > mag_thershold_end))
					{
						dotInput[i_det].dotCharacteristic = CLUTTER_POINT;
#ifdef DEBUG_LOG_ZQ
						my_printf("detect mag is too small %d", dotInput[i_det].dotId);
#endif // DEBUG_ZQ
					}
				}
			}

			//**** (2.3)调制反射点判断 ****//
//            if (isReflectPoint(range, doppler, mag, i_det, detNum, dotInput))
//            {
//#ifdef DEBUG_LOG_ZQ
//                my_printf("reflect point %d", dotInput[i_det].dotId);
//#endif // DEBUG_ZQ
//                dotInput[i_det].dotCharacteristic = REFLECTION_POINT;
//            }
//
			//**** (2.4)电源杂散点判断 ****//
//			if (isPowerSupplyPoint(range, doppler, mag, i_det, detNum, dotInput))
//			{
//#ifdef DEBUG_LOG_ZQ
//				my_printf("power point %d", dotInput[i_det].dotId);
//#endif // DEBUG_ZQ
//				dotInput[i_det].dotCharacteristic = POWER_SUPPLY_POINT;
//			}
#endif // DEMO_ACUR
		}
	}

	//*** 打印输出信息 ***//
	if (measInput->detNum > 0)
	{
		if (measInput->scanType == TWS_SCAN)
		{
			for (i_det = 0; i_det < measInput->detNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ_DEBUG
				my_printf("111111111-TWS-measIn-111111111 frameID[ %d ], curAziBeamRad[ %d ], detNum[ %d ], detId[ %d ],waveType[ %d ],dotCharac[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],wholeFinal[ %d ], tasOrInitFinal[ %d ], tasAndInitFinish[ %d ]\n", \
					measInput->frameID, measInput->curAziBeamRad, measInput->detNum, measInput->dotInput[i_det].dotId, measInput->dotInput[i_det].waveType, measInput->dotInput[i_det].dotCharacteristic,
					measInput->dotInput[i_det].range, measInput->dotInput[i_det].doppler, measInput->dotInput[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, \
					measInput->dotInput[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, measInput->dotInput[i_det].mag, \
					measInput->wholeSpaceScanCycleFinalBeamFlag, measInput->tasOrInitTrackFinalBeamFlag, measInput->tasAndInitTrackFinishBeamFlag);
#endif // DEBUG_LOG_HXJ
			}
		}
		else
		{
			for (i_det = 0; i_det < measInput->detNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ_DEBUG
				my_printf("111111111-TAS-measIn-111111111 frameID[ %d ], curAziBeamRad[ %d ], detNum[ %d ], detId[ %d ],waveType[ %d ],dotCharac[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],wholeFinal[ %d ], tasOrInitFinal[ %d ], tasAndInitFinish[ %d ]\n", \
					measInput->frameID, measInput->curAziBeamRad, measInput->detNum, measInput->dotInput[i_det].dotId, measInput->dotInput[i_det].waveType, measInput->dotInput[i_det].dotCharacteristic,
					measInput->dotInput[i_det].range, measInput->dotInput[i_det].doppler, measInput->dotInput[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, \
					measInput->dotInput[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, measInput->dotInput[i_det].mag, \
					measInput->wholeSpaceScanCycleFinalBeamFlag, measInput->tasOrInitTrackFinalBeamFlag, measInput->tasAndInitTrackFinishBeamFlag);
#endif // DEBUG_LOG_HXJ
			}
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("111111110-measIn-111111110 frameID[ %d ], detNum[ %d ], curAziBeamRad[ %d ], wholeFinal[ %d ], tasOrInitFinal[ %d ], tasAndInitFinish[ %d ]\n", \
			measInput->frameID, measInput->detNum, measInput->curAziBeamRad, measInput->wholeSpaceScanCycleFinalBeamFlag, measInput->tasOrInitTrackFinalBeamFlag, measInput->tasAndInitTrackFinishBeamFlag);
#endif // DEBUG_LOG_HXJ
	}

}

//**** (2.1)镜频抑制模块 ****//
void ifMirrorFreqPoint(uint8_t detNum, sDots_Cohesion_vector* dotInput, uint8_t* isMirrorFreqPoint)
{
	uint8_t iDet = 0, jDet = 0;
	int16_t dopplerChn_i, dopplerChn_j;
	float mag_i, mag_j;

	for (iDet = 0; iDet < detNum; iDet++)
	{
		mag_i = dotInput[iDet].mag;
		dopplerChn_i = dotInput[iDet].dopplerChn;

		if (isMirrorFreqPoint[iDet] == 1)
		{
			continue;
		}
		for (jDet = iDet + 1; jDet < detNum; jDet++)
		{
			mag_j = dotInput[jDet].mag;
			dopplerChn_j = dotInput[jDet].dopplerChn;
			if (isMirrorFreqPoint[jDet] == 1)
			{
				continue;
			}
			if (fabsf(dopplerChn_i + dopplerChn_j - NUM_DOPPLER_BIN) < MIR_FREQ_BIN_DIFF)
			{
				if (mag_i > (MIR_FREQ_MAG_RATIO * mag_j))
				{
					isMirrorFreqPoint[jDet] = 1;
				}
				else if (mag_j > (MIR_FREQ_MAG_RATIO * mag_i))
				{
					isMirrorFreqPoint[iDet] = 1;
					break;
				}
			}
		}
	}
}

//**** (2.3)调制反射点判断 ****//
uint8_t isReflectPoint(float range, float doppler, float mag, uint8_t n_det, uint8_t detNum, sDots_Cohesion_vector* dotInput)
{
	uint8_t iDet = 0;
	float range0, mag_i;
	float doppler0 = 0.f;
	uint8_t reflectFlag = 0;
	uint8_t multiRefNum = 11;
	uint8_t iReflect = 2;
	float dopplerAmbi = 0.f;
	float dopplerScope = 12.8823f;
	int n = 0;
	for (iDet = 0; iDet < detNum; iDet++)
	{
		range0 = dotInput[iDet].range;
		mag_i = dotInput[iDet].mag;
		doppler0 = dotInput[iDet].doppler;

		if (iDet == n_det)
			continue;
		if (range0 > range)
			continue;
		for (iReflect = 2; iReflect < multiRefNum; iReflect++)
		{
			n = (int)floor(((float)(iReflect)*doppler0 + 6.649f) / dopplerScope);
			if (n == 0)
			{
				dopplerAmbi = (float)(iReflect)*doppler0;
			}
			else
			{
				dopplerAmbi = (float)(iReflect)*doppler0 - (float)(n)*dopplerScope;
			}
			if (fabsf((float)iReflect * range0 - range) < 5.f + (iReflect - 2.f) * 2.f && \
				fabsf(dopplerAmbi - doppler) < 0.5f + (iReflect - 2.f) * 0.1f && \
				mag_i > mag)
			{
				reflectFlag = 1;
				break;
			}
		}
		if (reflectFlag == 1)
			break;
	}
	return reflectFlag;
}
/**
 * @b Description
 * @n
 *      This function is used to jundge whether measurement/target is within power supply
 *		clutter range interval.
 *
 * @param[in] measurement/target range
 *
 * @return
 *
 */
uint8_t isInPowerSupplyClutterRange(float range)
{
	if (!g_byDummyTest )
	{
		if (2 == g_byDevHwGen)
		{
#define POWER_CLUTTER_NUM  4U
			uint8_t i = 0;
			float rangeInterval[POWER_CLUTTER_NUM * 2] =
			{
						248.f, 250.f,
						371.f, 373.f,
						515.0f, 517.0f,
						588.0f, 597.0f,
			};

			for (i = 0; i < POWER_CLUTTER_NUM; i++)
			{
				if (range > rangeInterval[2 * i] && range < rangeInterval[2 * i + 1])
				{
					return 1;
				}
			}
		}
		else if (3 == g_byDevHwGen)
		{
#define POWER_CLUTTER_NUM  3U
			uint8_t i = 0;
			float rangeInterval[POWER_CLUTTER_NUM * 2] =
			{
				/*	512.0f,517.0f,
					872.0f,892.0f,
					899.0f,919.0f,*/
					983.0f,988.0f,
					1004.0f,1015.0f,
					1022.0f,1030.0f,
			};
			for (i = 0; i < POWER_CLUTTER_NUM; i++)
			{
				if (range > rangeInterval[2 * i] && range < rangeInterval[2 * i + 1])
				{
					return 1;
				}
			}

//#define POWER_CLUTTER_NUM  5U
//			uint8_t i = 0;
//			float rangeInterval[POWER_CLUTTER_NUM * 2] =
//			{
//						362.f, 372.f,
//						591.0f, 597.0f,
//						611.0f, 613.0f,
//						629.0f, 631.0f,
//						449.0f, 451.0f,
//			};
//
//			for (i = 0; i < POWER_CLUTTER_NUM; i++)
//			{
//				if (range > rangeInterval[2 * i] && range < rangeInterval[2 * i + 1])
//				{
//					return 1;
//				}
//			}
		}
		
	}
	return 0;
}

uint8_t isInPowerSupplyClutterRangeAndVelocity(float range, float doppler)
{
	if (!g_byDummyTest )
	{
		if (2 == g_byDevHwGen)
		{
#define POWER_CLUTTER_NUM  4U
			uint8_t i = 0;
			float rangeInterval[POWER_CLUTTER_NUM * 2] =
			{
						248.f, 250.f,
						371.f, 373.f,
						515.0f, 517.0f,
						588.0f, 597.0f,
			};
			for (i = 0; i < POWER_CLUTTER_NUM; i++)
			{
				if (range >= rangeInterval[2 * i] && range <= rangeInterval[2 * i + 1])
				{
					if (i < 1)
					{
						if (doppler >= -9.0 && doppler <= -8.0)
						{
							return 1;
						}
					}
					else if (i < 2)
					{
						if (fabsf(doppler) >= 6.5 && fabsf(doppler) <= 12)
						{
							return 1;
						}
					}
					else if (i < 3)
					{
						if (doppler >= 10.0 && doppler <= 11.5)
						{
							return 1;
						}
					}
					else 
					{
						if ( (fabsf(doppler) >= 1.2 && fabsf(doppler) <= 3.7) || (fabsf(doppler) >= 8.0 && fabsf(doppler) <= 10.0) )
						{
							return 1;
						}
					}
				}
			}

		}
		else if (3 == g_byDevHwGen)
		{
#define POWER_CLUTTER_NUM  3U
			uint8_t i = 0;
			float rangeInterval[POWER_CLUTTER_NUM * 2] =
			{
					/*	512.0f,517.0f,
						872.0f,892.0f,
						899.0f,919.0f,*/
					983.0f,988.0f,
					1004.0f,1015.0f,
					1022.0f,1030.0f,
			};
			for (i = 0; i < POWER_CLUTTER_NUM; i++)
			{
				if (range > rangeInterval[2 * i] && range < rangeInterval[2 * i + 1])
				{
					return 1;
				}
			}

//#define POWER_CLUTTER_NUM  13U
//			uint8_t i = 0;
//			float rangeInterval[POWER_CLUTTER_NUM * 2] =
//			{
//						512.0f,514.0f,
//						515.0f,517.0f,
//						872.0f,874.0f,
//						875.0f,877.0f,
//						878.0f,880.0f,
//						881.0f,883.0f,
//						884.0f,886.0f,
//						899.0f,901.0f,
//						902.0f,904.0f,
//						905.0f,907.0f,
//						908.0f,910.0f,
//						914.0f,916.0f,
//						917.0f,919.0f,
//			};
//			for (i = 0; i < POWER_CLUTTER_NUM; i++)
//			{
//				if (range >= rangeInterval[2 * i] && range <= rangeInterval[2 * i + 1])
//				{
//					return 1;
//				}
//			}

		}

	}
	return 0;
}


//**** (2.4)电源杂散点判断 ****//
uint8_t isPowerSupplyPoint(float range, float doppler, float mag, uint8_t n_det, uint8_t detNum, sDots_Cohesion_vector* dotInput)
{
	uint8_t iDet = 0;
	float range0;
	float doppler0 = 0.f;
	float range304 = 0.f;
	float doppler304 = 0.f;
	float range304_ = 0.f;
	float doppler304_ = 0.f;
	uint8_t powerSupplyFlag = 0;
	float dopplerAmbi = 0.f;
	uint8_t powerSupply304Flag = 0;
	float dopplerScope = DOPPLER_SCOPE_WAVETYPE_0;
	int n = 0;
	/*if (isInPowerSupplyClutterRange(range))*/
	if (isInPowerSupplyClutterRangeAndVelocity(range, doppler))
	{
		powerSupplyFlag = 1;
		return powerSupplyFlag;
	}
	/*for (iDet = 0; iDet < detNum; iDet++)
	{
		range304_ = dotInput[iDet].range;
		if (range304_ > 206.f && range304_ < 208.f)
		{
			doppler304_ = dotInput[iDet].doppler;
			if (fabsf(doppler304_) > fabsf(doppler304))
			{
				doppler304 = doppler304_;
				range304 = range304_;
				powerSupply304Flag = 1;
			}
		}
	}
	if (powerSupply304Flag == 0)
	{
		return powerSupplyFlag;
	}
	if (range > 150.f && powerSupply304Flag == 1)
	{
		for (iDet = 0; iDet < detNum; iDet++)
		{
			range0 = dotInput[iDet].range;
			doppler0 = dotInput[iDet].doppler;
			if (iDet == n_det)
				continue;
			if (range0 > range || range0 > 120.f)
				continue;
			n = (int)floor((doppler + doppler0 + DOPPLER_SCOPE_WAVETYPE_0 / 2.f) / dopplerScope);
			if (n == 0)
			{
				dopplerAmbi = doppler + doppler0;
			}
			else
			{
				dopplerAmbi = doppler + doppler0 - (float)(n)*dopplerScope;
			}

			if ((fabsf(range + range0 - 207.f) < 5.f && fabsf(dopplerAmbi - doppler304) < 1.f))
			{
				powerSupplyFlag = 1;
				break;
			}
			n = (int)floor((doppler - doppler0 + DOPPLER_SCOPE_WAVETYPE_0 / 2.f) / dopplerScope);
			if (n == 0)
			{
				dopplerAmbi = doppler - doppler0;
			}
			else
			{
				dopplerAmbi = doppler - doppler0 - (float)(n)*dopplerScope;
			}
			if ((fabsf(range - range0 - 304.f) < 5.f && fabsf(dopplerAmbi - doppler304) < 0.5f))
			{
				powerSupplyFlag = 1;
				break;
			}
		}
	}*/
	return powerSupplyFlag;
}



//** 3.dots clustering,just TWS ,TODO TAS **//
void dotsClustering(sMeasInput* measInput, sMeasProcessInst* measInst, sClusterDetect* clusterDet)
{
	if (measInput->scanType == TWS_SCAN)
	{
		dotsClustering_tws(measInput, &measInst->measConfigParam, clusterDet, measInst->TwsDotCoheStr);
	}
	else
	{
		dotsClustering_tas(measInput, &measInst->measConfigParam, clusterDet, measInst->TasDotCohesion);
	}
}

//**** TWS模式-(3.1)点迹聚类+存储模块 ****//
void dotsClustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_dot, i_det;

	if (TwsDotCoheStr->dotCoheNum == 0)
	{
		//**** TWS模式-波位内点迹聚类模块——存储区无目标簇时 ****//
		clusteringWithinBeam_tws(measInput, measConfigParam, clusterDet, TwsDotCoheStr);//the first frame
	}
	else
	{
		//**** TWS模式-波位间点迹聚类模块——存储区存有目标簇时****//
		//the other frame,当前帧检测点与之前目标簇聚类
		clusteringBetweenBeam_tws(measInput, measConfigParam, clusterDet, TwsDotCoheStr);//the other frame
	}

	//*** 打印输出信息 ***//
	if (TwsDotCoheStr->dotCoheNum > 0)
	{
		for (i_dot = 0; i_dot < TwsDotCoheStr->dotCoheNum; i_dot++)
		{
			for (i_det = 0; i_det < TwsDotCoheStr->TwsDotCohesion[i_dot].dotsNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ_DEBUG
				my_printf("222222222-TWS-CoheStr-222222222 frameID[ %d ], curAziBeamRad[ %d ], dotCoheNum[ %d ], clusterId[ %d ],coheFinish[ %d ],leftAziRad[ %d ], dotsNum[ %d ], dotId[ %d ], frameId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], dotCharac[ %d ]\n", \
					measInput->frameID, measInput->curAziBeamRad, TwsDotCoheStr->dotCoheNum, TwsDotCoheStr->TwsDotCohesion[i_dot].clusterId, TwsDotCoheStr->TwsDotCohesion[i_dot].cohesionFinish, TwsDotCoheStr->TwsDotCohesion[i_dot].leftBoundaryAziRad, TwsDotCoheStr->TwsDotCohesion[i_dot].dotsNum, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].dotId, TwsDotCoheStr->TwsDotCohesion[i_dot].frameId[i_det], TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].range, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].doppler, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].mag, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].dotCharacteristic);
#endif // DEBUG_LOG_HXJ
			}
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("222222220-TWS-CoheStr-222222220 frameID[ %d ], dotCoheNum[ %d ], curAziBeamRad[ %d ]\n", measInput->frameID, TwsDotCoheStr->dotCoheNum, measInput->curAziBeamRad);
#endif // DEBUG_LOG_HXJ
	}

	if (measInput->detNum > 0)
	{
		for (i_det = 0; i_det < measInput->detNum; i_det++)
		{
#ifdef DEBUG_LOG_HXJ_DEBUG
			my_printf("333333333-TWS-clustDet-333333333 frameID[ %d ], curAziBeamRad[ %d ], dsN[ %d ], dsNin[ %d ], ddN[ %d ],ddNin[ %d ],LN[ %d ], LNin[ %d ], clusterID[ %d ]\n", \
				measInput->frameID, measInput->curAziBeamRad, clusterDet->dots_store_Num, clusterDet->dots_store_NumIn, clusterDet->dots_dots_Num, \
				clusterDet->dots_dots_NumIn, clusterDet->lonelyNum, clusterDet->lonelyNumIn, clusterDet->clusterID[i_det]);
#endif // DEBUG_LOG_HXJ
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("333333330-TWS-clustDet-333333330 frameID[ %d ], curAziBeamRad[ %d ], dsN[ %d ], dsNin[ %d ], ddN[ %d ],ddNin[ %d ],LN[ %d ], LNin[ %d ]\n", \
			measInput->frameID, measInput->curAziBeamRad, clusterDet->dots_store_Num, clusterDet->dots_store_NumIn, clusterDet->dots_dots_Num, \
			clusterDet->dots_dots_NumIn, clusterDet->lonelyNum, clusterDet->lonelyNumIn);
#endif // DEBUG_LOG_HXJ
	}

}

//**** TWS模式-(3-1)波位内点迹聚类模块——存储区无目标簇时 ****//
void clusteringWithinBeam_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t n_det, detNum = 0;
	uint8_t dots_dots_ID, lonelyTarID = 0;
	uint8_t visited_in[MAX_NUM_DETECTS] = { 0 };

	sDots_Cohesion_vector* dotInput = measInput->dotInput;

	//**** 1、初始化 ****//
	detNum = measInput->detNum;
	dots_dots_ID = 0;//初始化目标簇总数，第1帧为0

	if (detNum > 0)
	{
		memset(clusterDet, 0, sizeof(sClusterDetect));//存储区不存在目标簇，初始化当前帧点迹聚类信息均为0
		memset(visited_in, CLUSTER_POINT_UNKNOWN, detNum * sizeof(uint8_t));

		//(1)当前帧点迹聚类状态初始化，Init the clusterID of points are CLUSTER_POINT_LONELY
		for (n_det = 0; n_det < detNum; n_det++)
		{
			clusterDet->clusterID[n_det] = CLUSTER_POINT_LONELY;
			if (dotInput[n_det].dotCharacteristic != REAL_POINT) //点迹属性判别
			{
				clusterDet->clusterID[n_det] = CLUSTER_POINT_UNKNOWN;
				visited_in[n_det] = CLUSTER_POINT_VISITED;
			}
		}
		//(2)当前帧点迹自行聚类，scan through all the points to find its neighbors,ID下标从0开始
		dots_dots_clustering_tws(measInput, measConfigParam, clusterDet, TwsDotCoheStr, visited_in, &dots_dots_ID);
		clusterDet->dots_store_Num = 0;//当前存储区无目标簇
		clusterDet->dots_dots_Num = dots_dots_ID;//可聚类目标数

		//(3)孤点查找
		for (n_det = 0; n_det < detNum; n_det++)
		{
			if (clusterDet->clusterID[n_det] == CLUSTER_POINT_LONELY)
			{
				lonelyTarID = lonelyTarID + 1;
			}
		}
		clusterDet->lonelyNum = lonelyTarID;//孤点目标

#ifdef DEBUG_LOG_HXJ
		//my_printf("detNum clustTarID lonelyTarID [%d %d %d]\n", detNum, clustTarID, lonelyTarID);
#endif // DEBUG_LOG_HXJ

		//**** 2、归类目标进行聚类 + 凝聚是否完成判断****//
		clusteringFunInBeam_tws(clusterDet, measInput, detNum, TwsDotCoheStr);
	}

}

//**** TWS模式-(3-1-1)当前帧点迹自行聚类，scan through all the points to find its neighbors,ID下标从0开始 ****//
void dots_dots_clustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, 
	sTwsDotCoheInst* TwsDotCoheStr, uint8_t* visited_in, uint8_t* dots_dots_ID)
{
	uint8_t detNum = 0;
	uint8_t n_det, i_current, i_neigh;
	uint8_t* neighLast;
	uint8_t* neighCurrent;
	uint8_t neighCount, newCount;
	uint8_t clustTar_ID;
	uint8_t scope[MAX_NUM_DETECTS] = { 0 };
	uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };

	sDots_Cohesion_vector* dotInput = measInput->dotInput;

	detNum = measInput->detNum;
	clustTar_ID = *dots_dots_ID;

	// scan through all the points to find its neighbors
	for (n_det = 0; n_det < detNum; n_det++)
	{
		if (dotInput[n_det].dotCharacteristic != REAL_POINT)
		{
			visited_in[n_det] = CLUSTER_POINT_VISITED;
			continue;
		}

		if (visited_in[n_det] != CLUSTER_POINT_VISITED)
		{
			visited_in[n_det] = CLUSTER_POINT_VISITED;

			neighCurrent = neighLast = neighbors;
			// scope is the local copy of visit
			memcpy(scope, visited_in, detNum * sizeof(uint8_t));

			neighCount = dot_dot_DBscan_findNeighbors(
				dotInput, measConfigParam, n_det, neighLast, detNum,
				scope, &newCount);

			/* The cluster consists of the point itself and its neighbours. */
			if (neighCount < MIN_NUM_POINTS_IN_CLUSTER - 1)
			{
				// This point is lonely point
				clusterDet->clusterID[n_det] = CLUSTER_POINT_LONELY;
			}
			else
			{
				// This point belongs to a New Cluster
				clusterDet->clusterID[n_det] = clustTar_ID;      // This point belong to this cluster，ID start from 0

				// tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
				for (i_neigh = 0; i_neigh < newCount; i_neigh++)
				{
					i_current = neighLast[i_neigh];//与当前n_det聚类的ID
					scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
				}
				neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位

				while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
				{
					// Explore the neighborhood
					i_current = *neighCurrent++;               // Take point from the neighborhood
					clusterDet->clusterID[i_current] = clustTar_ID; // All points from the neighborhood also belong to this cluster
					visited_in[i_current] = CLUSTER_POINT_VISITED;

					neighCount = dot_dot_DBscan_findNeighbors(
						dotInput, measConfigParam, i_current, neighLast, detNum,
						scope, &newCount);

					if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
					{
						for (i_neigh = 0; i_neigh < newCount; i_neigh++)
						{
							i_current = neighLast[i_neigh];
							scope[i_current] = CLUSTER_POINT_VISITED;
						}
						neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
					}
				}
				clustTar_ID++;                                //先计数，后累加，cluster ID +1
				if (clustTar_ID >= MAX_NUM_DETECTS)
				{
					return;
				}
			}
		}
	}
	*dots_dots_ID = (uint8_t)clustTar_ID;//点迹自身可聚类的目标总数

}

//**** TWS模式-(3-1-1-1)波位内DBScan点迹聚类子函数，完成****//
uint8_t dot_dot_DBscan_findNeighbors(sDots_Cohesion_vector* dotInput, sMeasConfigParam* measConfigParam,
	uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_in, uint8_t* newCount)
{
	uint8_t ii;
	uint8_t newCounttmp = 0;
	float tmpRange = 0, tmpVelocity = 0;
	float rangeN = dotInput[n].range;
	float velocityN = dotInput[n].doppler;
	float rangeThreshold, veloThreshold;

	rangeThreshold = measConfigParam->rCohGatingTws;
	veloThreshold = measConfigParam->dCohGatingTws;

	for (ii = 0; ii < numPoints; ii++)
	{
		if (dotInput[ii].dotCharacteristic != 0)
		{
			continue;
		}
		if (visited_in[ii] == CLUSTER_POINT_VISITED)
		{
			continue;
		}

		tmpRange = dotInput[ii].range;
		tmpVelocity = dotInput[ii].doppler;
		if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
		{
			/* Mark this point as a neighbour in the list of
			 * neighbours. Also increment the number of neighbours
			 * for this point. */
			*neigh = ii;
			neigh++;
			newCounttmp++;
		}
	}
	*newCount = (uint16_t)newCounttmp;
	return newCounttmp;
}

//**** TWS模式-(3-1-2)波位内点迹聚类处理函数****//
void clusteringFunInBeam_tws(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t dots_store_NumEnd = 0, dots_dots_NumEnd = 0, lonelyNumEnd = 0;
	uint8_t clustIdEnd = 0, clustIdEnd1 = 0, clustIdEnd2 = 0;
	uint8_t storeEmptyNum = 0;
	uint8_t* clustID = clusterDet->clusterID;
	uint8_t dots_store_Num = clusterDet->dots_store_Num;//与存储区目标簇可聚类的目标总数
	uint8_t dots_dots_Num = clusterDet->dots_dots_Num;//点迹自身可聚类的目标总数
	uint8_t lonelyNum = clusterDet->lonelyNum;//孤点目标总数

	memset(TwsDotCoheStr, 0, sizeof(sTwsDotCoheInst));

	//(1)更新存储区目标簇信息
	if (dots_dots_Num >= TWS_DOT_COHESION_NUM)//仅更新聚类点迹簇
	{
		dots_store_NumEnd = 0;
		dots_dots_NumEnd = TWS_DOT_COHESION_NUM;//有剩余新的点迹目标簇未加入存储区
		lonelyNumEnd = 0;
		clusterTargetFun_1st_tws(dots_dots_NumEnd, detNum, clustID, measInput, TwsDotCoheStr);
	}
	else //聚类点迹簇clustNum + 孤点lonelyNum
	{
		storeEmptyNum = TWS_DOT_COHESION_NUM - dots_dots_Num;//存储区空余空间总数

		 //(1-1)new target cluster —— the 1st frame
		dots_store_NumEnd = 0;
		dots_dots_NumEnd = dots_dots_Num;
		lonelyNumEnd = 0;

		clusterTargetFun_1st_tws(dots_dots_NumEnd, detNum, clustID, measInput, TwsDotCoheStr);

		//(1-2)lonely target —— the 1st frame
		clustIdEnd = dots_dots_Num;
		if (lonelyNum >= storeEmptyNum)//存储区可继续新增部分
		{
			lonelyNumEnd = storeEmptyNum;
		}
		else
		{
			lonelyNumEnd = lonelyNum;
		}
		lonelyTargetFun_tws(clustIdEnd, lonelyNumEnd, detNum, clustID, measInput, TwsDotCoheStr);
	}

	//(2)更新存储区目标簇总数
	TwsDotCoheStr->dotCoheNum = dots_store_NumEnd + dots_dots_NumEnd + lonelyNumEnd;//the number of target cluster

	//(3)更新放入存储区的各类点迹信息
	clusterDet->dots_store_NumIn = dots_store_NumEnd;//当前帧可聚类的目标--放入存储区的总数
	clusterDet->dots_dots_NumIn = dots_dots_NumEnd;//当前帧点迹自身可聚类的目标--放入存储区的总数
	clusterDet->lonelyNumIn = lonelyNumEnd;//当前帧孤点目标--放入存储区的总数
}

//**** TWS模式-(3-1-2-1)target cluster the 1st frame-- TWS
void clusterTargetFun_1st_tws(uint8_t clustNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_det, i_clustObj, i_clustID;
	uint8_t isUpdate = 0;
	sDots_Cohesion_vector* dotInput = measInput->dotInput;

	if (clustNumEnd > 0)
	{
		for (i_clustObj = 0; i_clustObj < clustNumEnd; i_clustObj++)
		{
			isUpdate = 0;
			i_clustID = TwsDotCoheStr->TwsDotCohesion[i_clustObj].dotsNum; //已有点迹总数     

			for (i_det = 0; i_det < detNum; i_det++)
			{
				if (clustID[i_det] == i_clustObj)
				{
					isUpdate = 1;
					if (i_clustID < POINTS_IN_CLUSTER_NUM)//i_det点迹可与i_clustObj目标簇聚类，且该目标簇的点迹总数未达上限
					{
						memcpy(&TwsDotCoheStr->TwsDotCohesion[i_clustObj].dotCluster[i_clustID], &dotInput[i_det], sizeof(sDots_Cohesion_vector));/* The dots structer contained in the current target cluster */
						TwsDotCoheStr->TwsDotCohesion[i_clustObj].frameId[i_clustID] = measInput->frameID;
						i_clustID++; //i_clustObj目标簇聚类点迹总数累加
					}
				}
			}

			if (isUpdate == 1)
			{
				TwsDotCoheStr->TwsDotCohesion[i_clustObj].clusterId = i_clustObj;
				TwsDotCoheStr->TwsDotCohesion[i_clustObj].dotsNum = i_clustID;//The number of dots in the current target cluster
				TwsDotCoheStr->TwsDotCohesion[i_clustObj].cohesionFinish = 0;//the first frame			
				TwsDotCoheStr->TwsDotCohesion[i_clustObj].leftBoundaryAziRad = measInput->curAziBeamRad;//dotInput[0].aziBeamRad,Update the beam azimuth of the left Boundary
				TwsDotCoheStr->TwsDotCohesion[i_clustObj].timestamp = measInput->timestamp;//Update the timestamp of the current target cluster
			}

		}
	}

}

//**** TWS模式-(3-1-2-2)lonely target the 1st frame-- TWS
void lonelyTargetFun_tws(uint8_t clustIdEnd, uint8_t lonelyNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_det, i_lonelyObj, isUpdate = 0;
	sDots_Cohesion_vector* dotInput = measInput->dotInput;

	if (lonelyNumEnd > 0)
	{
		for (i_lonelyObj = 0; i_lonelyObj < lonelyNumEnd; i_lonelyObj++)
		{
			isUpdate = 0;//新建目标簇是否更新标记
			for (i_det = 0; i_det < detNum; i_det++)
			{
				if (clustID[i_det] == CLUSTER_POINT_LONELY)
				{
					memcpy(&TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].dotCluster[0], &dotInput[i_det], sizeof(sDots_Cohesion_vector));/* The dots structer contained in the current target cluster */
					TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].frameId[0] = measInput->frameID;

					clustID[i_det] = clustIdEnd + i_lonelyObj;
					isUpdate = 1;
					break;
				}
			}
			if (isUpdate == 1)
			{
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].clusterId = clustIdEnd + i_lonelyObj;
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].dotsNum = 1;//The number of dots in the current target cluster
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].cohesionFinish = 0;//the lonely dots in current beam
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].leftBoundaryAziRad = measInput->curAziBeamRad;//dotInput[0].aziBeamRad,Update the beam azimuth of the left Boundary
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].timestamp = measInput->timestamp;//Update the timestamp of the current target cluster
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].unambigDone = 0;
				TwsDotCoheStr->TwsDotCohesion[clustIdEnd + i_lonelyObj].unambigVel = 0.f;		// init unambigVel while unambigDone = 0
			}
		}
	}

}

//**** TWS模式-(3-2)波位间点迹聚类模块——存储区存有目标簇时****//
void clusteringBetweenBeam_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t n_det, detNum = 0;
	uint8_t dots_store_ID, dots_dots_ID, lonelyTarID;
	uint8_t visited_between[MAX_NUM_DETECTS] = { 0 };

	uint8_t dotCoheNum = TwsDotCoheStr->dotCoheNum;
	sTwsDotCohesion* TwsDotCohesion = (sTwsDotCohesion*)TwsDotCoheStr->TwsDotCohesion;
	sDots_Cohesion_vector* dotInput = (sDots_Cohesion_vector*)measInput->dotInput;

	//**** 1、初始化 ****//
	detNum = measInput->detNum;
	dots_store_ID = 0;
	dots_dots_ID = 0;
	lonelyTarID = 0;

	if (detNum > 0)
	{
		memset(clusterDet, 0, sizeof(sClusterDetect));//存储区存有目标簇，初始化当前帧点迹聚类信息均为0
		memset(visited_between, CLUSTER_POINT_UNKNOWN, detNum * sizeof(uint8_t));

		//(1)当前帧点迹聚类状态初始化，Init the clusterID of points are CLUSTER_POINT_LONELY
		for (n_det = 0; n_det < detNum; n_det++)
		{
			clusterDet->clusterID[n_det] = CLUSTER_POINT_LONELY;
			if (dotInput[n_det].dotCharacteristic != REAL_POINT) //点迹属性判别
			{
				clusterDet->clusterID[n_det] = CLUSTER_POINT_UNKNOWN;
				visited_between[n_det] = CLUSTER_POINT_VISITED;
			}
		}

		//(2)当前帧点迹与存储区目标簇聚类，scan through all the points to find its neighbors
		if (dotCoheNum > 0)
		{
			dots_store_clustering_tws(measInput, measConfigParam, clusterDet, TwsDotCoheStr, visited_between, &dots_store_ID);
			clusterDet->dots_store_Num = dots_store_ID;//与存储区目标簇可聚类的目标总数,理论上=存储区已有目标簇总数
		}

		//(3)当前帧点迹与存储区没有聚类，但可自行聚类
		dots_dots_ID = TWS_DOT_COHESION_NUM;//点迹自身可聚类的目标ID从存储区上限开始
		dots_dots_clustering_tws(measInput, measConfigParam, clusterDet, TwsDotCoheStr, visited_between, &dots_dots_ID);
		clusterDet->dots_dots_Num = (uint8_t)(dots_dots_ID - TWS_DOT_COHESION_NUM);//点迹自身可聚类的目标总数  

		//(4)孤点查找
		for (n_det = 0; n_det < detNum; n_det++)
		{
			if (clusterDet->clusterID[n_det] == CLUSTER_POINT_LONELY)
			{
				lonelyTarID = lonelyTarID + 1;
			}
		}
		clusterDet->lonelyNum = lonelyTarID;//孤点目标

#ifdef DEBUG_LOG_HXJ
		//my_printf("detNum clustTarID lonelyTarID [%d %d %d]\n", detNum, clustTarID, lonelyTarID);
#endif // DEBUG_LOG_HXJ

		//**** 2、归类目标进行聚类 + 凝聚是否完成判断 ****//
		clusteringFunBetweenBeam_tws(clusterDet, measInput, detNum, TwsDotCoheStr);
	}

}

//**** TWS模式-(3-2-1)当前帧点迹与存储区目标簇聚类
void dots_store_clustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr, uint8_t* visited_between, uint8_t* dots_store_ID)
{
	uint8_t detNum, dotsNum = 0, clustTar_Num = 0;
	uint8_t n_cluster, n_dot;
	uint8_t i_current, i_neigh;
	uint8_t* neighLast;
	uint8_t* neighCurrent;
	uint8_t neighCount, newCount;
	uint8_t clustTar_ID;
	uint8_t scope[MAX_NUM_DETECTS] = { 0 };
	uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };
	uint8_t dotCoheNum = TwsDotCoheStr->dotCoheNum;
	sTwsDotCohesion* TwsDotCohesion = TwsDotCoheStr->TwsDotCohesion;
	sDots_Cohesion_vector* dotCluster;
	detNum = measInput->detNum;

	for (n_cluster = 0; n_cluster < dotCoheNum; n_cluster++)
	{
		dotsNum = TwsDotCohesion[n_cluster].dotsNum;
		dotCluster = TwsDotCohesion[n_cluster].dotCluster;
		clustTar_ID = TwsDotCohesion[n_cluster].clusterId;//current target cluster	
		clustTar_Num++; //依次遍历存储区目标簇

		if (dotsNum > 0)
		{
			for (n_dot = 0; n_dot < dotsNum; n_dot++)//依次遍历各目标簇已聚类的点迹
			{
				neighCurrent = neighLast = neighbors;
				// scope is the local copy of visit
				memcpy(scope, visited_between, detNum * sizeof(uint8_t));

				neighCount = dot_store_DBscan_findNeighbors(
					measInput, measConfigParam, dotCluster, n_dot, 
					&TwsDotCohesion[n_cluster].unambigDone, &TwsDotCohesion[n_cluster].unambigVel,
					neighLast,scope, &newCount);

				/* The cluster consists of the point itself and its neighbours. */
				if (neighCount >= 1)
				{
					// tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
					for (i_neigh = 0; i_neigh < newCount; i_neigh++)
					{
						i_current = neighLast[i_neigh];//与当前n_det聚类的ID
						scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
					}
					neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位

					while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
					{
						// Explore the neighborhood
						i_current = *neighCurrent++;               // Take point from the neighborhood
						clusterDet->clusterID[i_current] = clustTar_ID; // All points from the neighborhood also belong to this cluster
						visited_between[i_current] = CLUSTER_POINT_VISITED;

						//neighCount = dot_store_DBscan_findNeighbors(
						//    measInput, measConfigParam, dotCluster, i_current, neighLast,
						//    scope, &newCount);

						neighCount = dot_dot_DBscan_findNeighbors(
							measInput->dotInput, measConfigParam, i_current, neighLast, detNum,
							scope, &newCount);

						if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
						{
							for (i_neigh = 0; i_neigh < newCount; i_neigh++)
							{
								i_current = neighLast[i_neigh];
								scope[i_current] = CLUSTER_POINT_VISITED;
							}
							neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
						}
					}
					if (clustTar_ID >= TWS_DOT_COHESION_NUM)
					{
						return;
					}
				}
			}
		}
	}

	*dots_store_ID = (uint8_t)clustTar_Num;//clustTar_ID
}

//**** TWS模式-(3-2-1-1)波位间DBScan点迹聚类子函数，完成 ****//
uint8_t dot_store_DBscan_findNeighbors(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sDots_Cohesion_vector* dotCluster, uint8_t n_dot,
	uint8_t* unambigDone, float* unambigVel, uint8_t* neigh, uint8_t* visited, uint8_t* newCount)
{
	uint8_t i_det;
	uint8_t newCounttmp = 0;
	float tmpRange = 0, tmpVelocity = 0;
	float rangeN = dotCluster[n_dot].range;
	float velocityN = dotCluster[n_dot].doppler;
	float rangeThreshold, veloThreshold;

	uint8_t detNum = measInput->detNum;
	sDots_Cohesion_vector* dotInput = (sDots_Cohesion_vector*)measInput->dotInput;

	if (measInput->scanType == TWS_SCAN)
	{
		rangeThreshold = measConfigParam->rCohGatingTws;
		veloThreshold = measConfigParam->dCohGatingTws;
	}
	else
	{
		rangeThreshold = measConfigParam->rCohGatingTas;
		veloThreshold = measConfigParam->dCohGatingTas;
	}

	for (i_det = 0; i_det < detNum; i_det++)
	{
		if (visited[i_det] == CLUSTER_POINT_VISITED)
		{
			continue;
		}

		tmpRange = dotInput[i_det].range;
		tmpVelocity = dotInput[i_det].doppler;
#ifdef DEBUG_UNAMBIVEL
		my_printf("Before: dot range unambDone vel [%.2f %d %.2f], measVel %.2f", rangeN, *unambigDone, *unambigVel, tmpVelocity);
#endif // DEBUG_UNAMBIVEL

		if (gVelunambigType == STEP_SCAN)
		{
			if (fabsf(tmpRange - rangeN) <= rangeThreshold)
			{
				if (*unambigDone == 1)
				{
					tmpVelocity = tracking_velocityUnambigious(dotInput[i_det].waveType, *unambigVel, tmpVelocity, 0);
					velocityN = *unambigVel;
#ifdef DEBUG_UNAMBIVEL
					my_printf("Afer: dot unambDone vel [%d %.2f]", *unambigDone, *unambigVel);
#endif // DEBUG_UNAMBIVEL
				}
				else
				{
					if (dotInput[i_det].waveType != dotCluster[n_dot].waveType)
					{
						*unambigDone = tracking_velocityUnambigiousInit(&velocityN, &tmpVelocity, dotInput[i_det].waveType);
						*unambigVel = velocityN;					//dots velocity		
#ifdef DEBUG_UNAMBIVEL
						my_printf("Afer: dot unambDone vel [%d %.2f]", *unambigDone, *unambigVel);
#endif // DEBUG_UNAMBIVEL//dotCluster[n_dot].doppler = velocityN;		//unambigious doppler of measurements in cluster
						//dotInput[i_det].doppler = tmpVelocity;		//unambigious doppler of measurements
					}
				}
			}
		}
		if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
		{
			/* Mark this point as a neighbour in the list of
			 * neighbours. Also increment the number of neighbours
			 * for this point. */
			*neigh = i_det;
			neigh++;
			newCounttmp++;
		}
	}
	*newCount = (uint16_t)newCounttmp;
	return newCounttmp;
}

//**** TWS模式-(3-2-2)波位间点迹聚类处理函数 ****//
void clusteringFunBetweenBeam_tws(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t dots_store_NumEnd = 0, dots_dots_NumEnd = 0, lonelyNumEnd = 0;
	uint8_t dotClustId = 0, clustIdEnd = 0, clustIdEnd1 = 0, clustIdEnd2 = 0;
	uint8_t storeEmptyNum = 0;
	uint8_t* clustID = clusterDet->clusterID;
	uint8_t dots_store_Num = clusterDet->dots_store_Num;//与存储区目标簇可聚类的目标总数
	uint8_t dots_dots_Num = clusterDet->dots_dots_Num;//点迹自身可聚类的目标总数
	uint8_t lonelyNum = clusterDet->lonelyNum;//孤点目标总数


	//(1)更新存储区目标簇信息
	if (dots_store_Num < TWS_DOT_COHESION_NUM) //目标簇存储区未满
	{
		storeEmptyNum = TWS_DOT_COHESION_NUM - dots_store_Num;//存储区空余空间总数

		//(1-1)target cluster，先更新存储区已有目标簇点迹信息,并做凝聚条件判断，target cluster the others frame-- TWS 
		dots_store_NumEnd = dots_store_Num;
		dots_dots_NumEnd = 0;
		lonelyNumEnd = 0;
		clusterTargetFun_store_tws(dots_store_NumEnd, detNum, clustID, measInput, TwsDotCoheStr);

		//(1-2)remain dots cluster + lonely target
		dotClustId = TWS_DOT_COHESION_NUM;//当前帧新的点迹目标簇识别起始ID
		if (dots_dots_Num >= storeEmptyNum)//存储区可继续新增（部分新的点迹目标簇）
		{
			clustIdEnd = dots_store_Num;//点迹目标簇加入存储区的起始ID
			dots_dots_NumEnd = storeEmptyNum;//存储区可继续添加的点迹目标簇总数=存储区空余目标簇空间个数
			clusterTargetFun_new_tws(clustIdEnd, dots_dots_NumEnd, dotClustId, detNum, clustID, measInput, TwsDotCoheStr);//ID判断从(30+ii)开始，函数更新
		}
		else//存储区可新增（全部新的点迹目标簇+孤点）
		{
			clustIdEnd1 = dots_store_Num;//剩余目标簇存储起始下标
			dots_dots_NumEnd = dots_dots_Num;//可加入存储区的目标簇总数

			clustIdEnd2 = dots_store_Num + dots_dots_Num;//孤点存储起始下标

			if (dots_dots_Num + lonelyNum > storeEmptyNum)//新的目标簇+孤点总数 > 存储区空余空间数，存储区加入（全部新的点迹目标簇+部分孤点）
			{
				lonelyNumEnd = storeEmptyNum - dots_dots_Num;//可加入存储区的孤点总数
			}
			else//新的目标簇+孤点总数 <= 存储区空余空间数，存储区加入（全部新的点迹目标簇+全部孤点）
			{
				lonelyNumEnd = lonelyNum;//可加入存储区的孤点总数
			}
			clusterTargetFun_new_tws(clustIdEnd1, dots_dots_NumEnd, dotClustId, detNum, clustID, measInput, TwsDotCoheStr);//ID判断从(30+ii)开始，函数更新
			lonelyTargetFun_tws(clustIdEnd2, lonelyNumEnd, detNum, clustID, measInput, TwsDotCoheStr);
		}
	}
	else//目标簇存储区已满，仅更新可与其中目标簇聚类的点迹信息
	{
		dots_store_NumEnd = TWS_DOT_COHESION_NUM;
		dots_dots_NumEnd = 0;
		lonelyNumEnd = 0;
		clusterTargetFun_store_tws(dots_store_NumEnd, detNum, clustID, measInput, TwsDotCoheStr);
	}

	//(2)更新存储区目标簇总数
	TwsDotCoheStr->dotCoheNum = dots_store_NumEnd + dots_dots_NumEnd + lonelyNumEnd;//the number of target cluster

	//(3)更新放入存储区的各类点迹信息
	clusterDet->dots_store_NumIn = dots_store_NumEnd;//当前帧可聚类的目标--放入存储区的总数
	clusterDet->dots_dots_NumIn = dots_dots_NumEnd;//当前帧点迹自身可聚类的目标--放入存储区的总数
	clusterDet->lonelyNumIn = lonelyNumEnd;//当前帧孤点目标--放入存储区的总数
}

//**** TWS模式-(3-2-2-1)存储区目标簇更新,target cluster the others frame
void clusterTargetFun_store_tws(uint8_t clustNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_det, i_clustObj, i_clustID = 0;
	uint8_t clusterId = 0;
	uint8_t isUpdate = 0;
	sDots_Cohesion_vector* dotInput = measInput->dotInput;
	sTwsDotCohesion* TwsDotCohesion = (sTwsDotCohesion*)TwsDotCoheStr->TwsDotCohesion;

	if (clustNumEnd > 0)
	{
		for (i_clustObj = 0; i_clustObj < clustNumEnd; i_clustObj++)
		{
			clusterId = TwsDotCohesion[i_clustObj].clusterId;//存储区目标簇ID
			i_clustID = TwsDotCohesion[i_clustObj].dotsNum; //已有点迹总数
			isUpdate = 0;
			//****** (1)与目标簇聚类点迹存储 ******//
			for (i_det = 0; i_det < detNum; i_det++)
			{
				if (clustID[i_det] == clusterId)
				{
					isUpdate = 1;
					if (i_clustID < POINTS_IN_CLUSTER_NUM)//i_det点迹可与i_clustObj目标簇聚类，且该目标簇的点迹总数未达上限
					{
						memcpy(&TwsDotCohesion[i_clustObj].dotCluster[i_clustID], &dotInput[i_det], sizeof(sDots_Cohesion_vector));/* The dots structer contained in the current target cluster */
						TwsDotCohesion[i_clustObj].frameId[i_clustID] = measInput->frameID;
						i_clustID++; //i_clustObj目标簇聚类点迹总数累加
					}
				}
			}

			//****** (4)目标簇状态参数更新 ******//
			TwsDotCohesion[i_clustObj].clusterId = clusterId;
			TwsDotCohesion[i_clustObj].dotsNum = i_clustID;//The number of dots in the current target cluster
			TwsDotCohesion[i_clustObj].timestamp = measInput->timestamp;//Update the timestamp of the current target cluster
			TwsDotCohesion[i_clustObj].cohesionFinish = 0;
			if (isUpdate == 1)
			{
				TwsDotCohesion[i_clustObj].leftBoundaryAziRad = measInput->curAziBeamRad;//dotInput[0].aziBeamRad,更新扫描波束左边界方位角=当前扫描波束方位角
			}
		}
	}

}


//**** TAS模式-(3.2)点迹聚类+存储模块 ****//
void dotsClustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion)
{
	//**** TAS模式-当前帧检测点与TAS跟踪目标聚类 ****//
	uint8_t i_det, detNum = 0;
	uint8_t dots_tracker_ID = 0, dots_store_ID = 0, lonelyTarID = 0;
	uint8_t visited[MAX_NUM_DETECTS] = { 0 };

	sDots_Cohesion_vector* dotInput = (sDots_Cohesion_vector*)measInput->dotInput;

	//**** 1、初始化 ****//
	detNum = measInput->detNum;

	if (detNum > 0)
	{
		memset(clusterDet, 0, sizeof(sClusterDetect));//存储区存有目标簇，初始化当前帧点迹聚类信息均为0
		memset(visited, CLUSTER_POINT_UNKNOWN, detNum * sizeof(uint8_t));

		//(1)当前帧点迹聚类状态初始化，Init the clusterID of points are CLUSTER_POINT_LONELY
		for (i_det = 0; i_det < detNum; i_det++)
		{
			clusterDet->clusterID[i_det] = CLUSTER_POINT_LONELY;
			if (dotInput[i_det].dotCharacteristic != REAL_POINT) //点迹属性判别
			{
				clusterDet->clusterID[i_det] = CLUSTER_POINT_UNKNOWN;
				visited[i_det] = CLUSTER_POINT_VISITED;
			}
		}

		//(2)当前帧点迹与TAS跟踪目标聚类
		dots_tracker_ID = 0;//点迹与TAS跟踪目标可聚类的目标ID=0
		dot_tracker_clustering_tas(measInput, measConfigParam, clusterDet, TasDotCohesion, visited, &dots_tracker_ID);
		clusterDet->dots_tracker_Num = dots_tracker_ID;//点迹与TAS跟踪目标可聚类的目标总数=1，仅TAS目标一个 

		//(3)当前帧点迹与存储区目标簇聚类，scan through all the points to find its neighbors
		dots_store_ID = 1;//点迹与存储区目标簇聚类可聚类的目标ID=1,只打标记，不参与聚类和凝聚
		dots_store_clustering_tas(measInput, measConfigParam, clusterDet, TasDotCohesion, visited, &dots_store_ID);
		clusterDet->dots_store_Num = dots_store_ID;//存储区目标簇点迹与当前帧点迹可聚类的总数

		//(4)孤点查找，未能与当前TAS目标聚类的点迹
		for (i_det = 0; i_det < detNum; i_det++)
		{
			if (clusterDet->clusterID[i_det] == CLUSTER_POINT_LONELY)
			{
				lonelyTarID = lonelyTarID + 1;
			}
		}
		clusterDet->lonelyNum = lonelyTarID;//孤点目标总数

#ifdef DEBUG_LOG_HXJ
		//my_printf("detNum clustTarID lonelyTarID [%d %d %d]\n", detNum, clustTarID, lonelyTarID);
#endif // DEBUG_LOG_HXJ

		//**** 2、归类目标进行聚类 ****//
		clusteringFunBetweenBeam_tas(clusterDet, measInput, detNum, TasDotCohesion);
	}

	//*** 打印输出信息 ***//
	if (TasDotCohesion[0].dotsNum > 0)
	{
		for (i_det = 0; i_det < TasDotCohesion[0].dotsNum; i_det++)
		{
#ifdef DEBUG_LOG_HXJ_DEBUG
			my_printf("222222222-TAS-CoheStr-222222222 frameID[ %d ], curAziBeamRad[ %d ], coheFinish[ %d ], frameId[ %d ],dotsNum[ %d ], dotId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], dotCharac[ %d ]\n", \
				measInput->frameID, measInput->curAziBeamRad, TasDotCohesion[0].cohesionFinish, TasDotCohesion[0].frameId[i_det], TasDotCohesion[0].dotsNum, TasDotCohesion[0].dotCluster[i_det].dotId, TasDotCohesion[0].dotCluster[i_det].range, \
				TasDotCohesion[0].dotCluster[i_det].doppler, TasDotCohesion[0].dotCluster[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, TasDotCohesion[0].dotCluster[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, \
				TasDotCohesion[0].dotCluster[i_det].mag, TasDotCohesion[0].dotCluster[i_det].dotCharacteristic);
#endif // DEBUG_LOG_HXJ_DEBUG
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("222222220-TAS-CoheStr-222222220 frameID[ %d ], dotsNum[ %d ], curAziBeamRad[ %d ], tasTargetId[ %d ]\n", measInput->frameID, TasDotCohesion[0].dotsNum, measInput->curAziBeamRad, measInput->tasTrackStr[0].tasTargetId);
#endif // DEBUG_LOG_HXJ_DEBUG
	}

	if (measInput->detNum > 0)
	{
		for (i_det = 0; i_det < measInput->detNum; i_det++)
		{
#ifdef DEBUG_LOG_HXJ_DEBUG
			my_printf("333333333-TAS-clustDet-333333333 frameID[ %d ], curAziBeamRad[ %d ], dtN[ %d ], dsN[ %d ], clusterID[ %d ]\n", \
				measInput->frameID, measInput->curAziBeamRad, clusterDet->dots_tracker_Num, clusterDet->dots_store_Num, clusterDet->clusterID[i_det]);
#endif // DEBUG_LOG_HXJ_DEBUG
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("333333330-TAS-clustDet-333333330 frameID[ %d ], curAziBeamRad[ %d ], dtN[ %d ], dsN[ %d ]\n", \
			measInput->frameID, measInput->curAziBeamRad, clusterDet->dots_tracker_Num, clusterDet->dots_store_Num);
#endif // DEBUG_LOG_HXJ_DEBUG
	}

}

//**** TAS模式-(3-2-1)当前帧点迹与TAS跟踪目标聚类 
void dot_tracker_clustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion, uint8_t* visited, uint8_t* dots_tracker_ID)
{
	uint8_t detNum, clustTar_Num = 0;
	uint8_t i_current, i_neigh;
	uint8_t* neighLast;
	uint8_t* neighCurrent;
	uint8_t neighCount, newCount;
	uint8_t clustTar_ID;
	uint8_t scope[MAX_NUM_DETECTS] = { 0 };
	uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };

	sDots_Cohesion_vector* dotCluster;
	detNum = measInput->detNum;//当前帧输入点迹总数
	dotCluster = TasDotCohesion->dotCluster;
	clustTar_ID = *dots_tracker_ID;//current target cluster


	neighCurrent = neighLast = neighbors;
	// scope is the local copy of visit
	memcpy(scope, visited, detNum * sizeof(uint8_t));

	neighCount = dot_tracker_DBscan_findNeighbors(
		measInput, measConfigParam, dotCluster, neighLast,
		scope, &newCount);

	/* The cluster consists of the point itself and its neighbours. */
	if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
	{
		// tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
		for (i_neigh = 0; i_neigh < newCount; i_neigh++)
		{
			i_current = neighLast[i_neigh];//与当前n_det聚类的ID
			scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
		}
		neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位

		while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
		{
			// Explore the neighborhood
			i_current = *neighCurrent++;               // Take point from the neighborhood
			clusterDet->clusterID[i_current] = clustTar_ID; // All points from the neighborhood also belong to this cluster
			visited[i_current] = CLUSTER_POINT_VISITED;
			clustTar_Num++; //当前帧点迹与TAS目标可聚类的总数

			//neighCount = dot_store_DBscan_findNeighbors(
			//    measInput, measConfigParam, dotCluster, i_current, neighLast,
			//    scope, &newCount);

			neighCount = dot_dot_DBscan_findNeighbors(
				measInput->dotInput, measConfigParam, i_current, neighLast, detNum,
				scope, &newCount);

			if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
			{
				for (i_neigh = 0; i_neigh < newCount; i_neigh++)
				{
					i_current = neighLast[i_neigh];
					scope[i_current] = CLUSTER_POINT_VISITED;
				}
				neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
			}
		}
		if (clustTar_ID >= 1)//TAS模式下，存储区只存与当前TAS跟踪目标可聚类的点迹，仅1个目标簇，index=0
		{
			return;
		}
	}

	*dots_tracker_ID = (uint8_t)clustTar_Num;//与TAS目标可聚类的点迹总数
}

//**** TAS模式-(3-2-1-1)跟踪目标+检测点DBScan点迹聚类子函数 ****//
uint8_t dot_tracker_DBscan_findNeighbors(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sDots_Cohesion_vector* dotCluster, uint8_t* neigh, uint8_t* visited, uint8_t* newCount)
{
	uint8_t i_det;
	uint8_t newCounttmp = 0;
	float tmpRange = 0, tmpVelocity = 0;
	float rangeN, velocityN;
	float rangeThreshold, veloThreshold;
	uint8_t detNum = measInput->detNum;
	sDots_Cohesion_vector* dotInput = (sDots_Cohesion_vector*)measInput->dotInput;
	sMeasurement_enum_wavetype waveType = measInput->dotInput->waveType;

	rangeN = measInput->tasTrackStr->range;
	velocityN = measInput->tasTrackStr->doppler;

	rangeThreshold = measConfigParam->rDetTrackTas;
	veloThreshold = measConfigParam->dDetTrackTas;

	if (gVelunambigType == STEP_SCAN)
	{
		if (gCfmBeamFlag == 1)
		{
			veloThreshold = MAXVEL;
		}
	}
	else if (gVelunambigType == WHOLE_SCAN)
	{
		if (measInput->tasTrackStr->stateType == UNSTABLE_TRAJ)
		{
			/** TODO: velocity unambigious */
			veloThreshold = MAXVEL;
		}
	}
	
	for (i_det = 0; i_det < detNum; i_det++)
	{
		if (visited[i_det] == CLUSTER_POINT_VISITED)
		{
			continue;
		}
		tmpRange = dotInput[i_det].range;
		tmpVelocity = dotInput[i_det].doppler;

#ifdef DEBUG_LOG_HXJ//DEBUG_LOG_ZQ
		//my_printf("tas dbscan i_det[ %d ], vmeas[ %.2f ]\n", i_det, tmpVelocity);
#endif // DEBUG_LOG_ZQ
		if (gVelunambigType == WHOLE_SCAN)
		{
			tmpVelocity = tracking_velocityUnambigious(waveType, velocityN, tmpVelocity, 0);
		}
		

#ifdef DEBUG_LOG_HXJ//DEBUG_LOG_ZQ
		my_printf("tas dbscan i_det[ %d ], vmeas[ %.2f ], vtrk[ %.2f ], dr dv[ %.2f, %.2f ]\n", i_det, tmpVelocity, velocityN, fabsf(tmpRange - rangeN), fabsf(tmpVelocity - velocityN));
#endif // DEBUG_LOG_ZQ
		else if (gVelunambigType == STEP_SCAN)
		{
			tmpVelocity = tracking_velocityUnambigious(dotInput[i_det].waveType, velocityN, tmpVelocity, 0);
		}
		if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
		{
			/* Mark this point as a neighbour in the list of
			 * neighbours. Also increment the number of neighbours
			 * for this point. */
			*neigh = i_det;
			neigh++;
			newCounttmp++;
		}
	}
	*newCount = (uint16_t)newCounttmp;
	return newCounttmp;
}

//**** TAS模式-(3-2-2)当前帧点迹与存储区目标簇聚类
void dots_store_clustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet,
	sTasDotCohesion* TasDotCohesion, uint8_t* visited, uint8_t* dots_store_ID)
{
	uint8_t n_dot;
	uint8_t detNum, dotsNum = 0, clustTar_Num = 0;
	uint8_t i_current, i_neigh;
	uint8_t* neighLast;
	uint8_t* neighCurrent;
	uint8_t neighCount, newCount;
	uint8_t clustTar_ID;
	uint8_t scope[MAX_NUM_DETECTS] = { 0 };
	uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };

	sDots_Cohesion_vector* dotCluster;
	detNum = measInput->detNum;//当前帧输入点迹总数
	dotsNum = TasDotCohesion->dotsNum;//存储区已于TAS目标聚类的点迹总数
	dotCluster = TasDotCohesion->dotCluster;
	clustTar_ID = *dots_store_ID;//current target cluster

	//clustTar_Num++; //依次遍历存储区目标簇
	if (gVelunambigType == STEP_SCAN)
	{
		if (g_scanType == TAS_SCAN)			// the tas target's velocity is unambigious
		{
			TasDotCohesion->unambigDone = 1;
			TasDotCohesion->unambigVel = measInput->tasTrackStr->doppler;
		}
	}
	if (dotsNum > 0)
	{
		for (n_dot = 0; n_dot < dotsNum; n_dot++)//依次遍历各目标簇已聚类的点迹
		{
			neighCurrent = neighLast = neighbors;
			// scope is the local copy of visit
			memcpy(scope, visited, detNum * sizeof(uint8_t));

			neighCount = dot_store_DBscan_findNeighbors(
				measInput, measConfigParam, dotCluster, n_dot,
				&TasDotCohesion->unambigDone, &TasDotCohesion->unambigVel,
				neighLast,	scope, &newCount);

			/* The cluster consists of the point itself and its neighbours. */
			//if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
			//{
			//	//clustTar_Num++; //依次遍历存储区点迹，与当前帧可聚类的计数+1
			//	// tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
			//	for (i_neigh = 0; i_neigh < newCount; i_neigh++)
			//	{
			//		i_current = neighLast[i_neigh];//与当前n_det聚类的ID
			//		scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
			//	}
			//	neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位

			//	while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
			//	{
			//		// Explore the neighborhood
			//		i_current = *neighCurrent++;               // Take point from the neighborhood
			//		clusterDet->clusterID[i_current] = clustTar_ID; // All points from the neighborhood also belong to this cluster
			//		visited[i_current] = CLUSTER_POINT_VISITED;
			//		clustTar_Num++; //当前帧点迹与存储区可聚类的总数

			//		neighCount = dot_dot_DBscan_findNeighbors(
			//			measInput->dotInput, measConfigParam, i_current, neighLast, detNum,
			//			scope, &newCount);

			//		if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
			//		{
			//			for (i_neigh = 0; i_neigh < newCount; i_neigh++)
			//			{
			//				i_current = neighLast[i_neigh];
			//				scope[i_current] = CLUSTER_POINT_VISITED;
			//			}
			//			neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
			//		}
			//	}
			//	//if (clustTar_ID >= TWS_DOT_COHESION_NUM)//TAS模式下，存储区只存与当前TAS跟踪目标可聚类的点迹，仅1个目标簇，index=0
			//	//{
			//	//    return;
			//	//}
			//}
		}
	}

	*dots_store_ID = (uint8_t)clustTar_Num;
}

//**** TAS模式-(3-2-3)与TAS目标聚类点迹存储 ****//
void clusteringFunBetweenBeam_tas(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTasDotCohesion* TasDotCohesion)
{
	uint8_t i_det, i_clustID = 0;
	uint8_t clusterId;
	sDots_Cohesion_vector* dotInput = measInput->dotInput;
	uint8_t* clustID = clusterDet->clusterID;

	clusterId = 0;//TAS存储区仅一个目标簇，ID=0
	i_clustID = TasDotCohesion[0].dotsNum; //已有点迹总数

	//****** (1)与目标簇聚类点迹存储 ******//
	for (i_det = 0; i_det < detNum; i_det++)
	{
		if ((clustID[i_det] == clusterId) && (i_clustID < POINTS_IN_CLUSTER_NUM))//i_det点迹可与TAS目标簇聚类，且该目标簇的点迹总数未达上限
		{
			memcpy(&TasDotCohesion[0].dotCluster[i_clustID], &dotInput[i_det], sizeof(sDots_Cohesion_vector));/* The dots structer contained in the current target cluster */
			TasDotCohesion[0].frameId[i_clustID] = measInput->frameID;
			i_clustID++; //TAS目标簇聚类点迹总数累加
		}
	}

	//****** (3)目标簇状态参数更新 ******//
	TasDotCohesion[0].clusterId = clusterId;
	TasDotCohesion[0].dotsNum = i_clustID;//The number of dots in the current target cluster
	TasDotCohesion[0].timestamp = measInput->timestamp;//Update the timestamp of the current target cluster
	TasDotCohesion[0].cohesionFinish = 0;

}




//** 4.judge whether target scan is finished,dots cohesion and output, distinguish TWS/TAS mode **//
void dotsCohesionAndOutput(sMeasInput* measInput, sMeasProcessInst* measInst, sMeasConfigParam* measConfigParam)
{
	if (measInput->scanType == TWS_SCAN)
	{
		dotsCohesionAndOutput_tws(measInput, measInst->TwsDotCoheStr, measInst->measOutput, measConfigParam);
	}
	else
	{
		dotsCohesionAndOutput_tas(measInput, measInst->TasDotCohesion, measInst->measOutput);
	}
}

//**** TWS模式-(4.1)判断目标簇是否满足测角条件，更新cohesionFinish标记+点迹凝聚+测角函数 ****//
void dotsCohesionAndOutput_tws(sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr, sMeasOutput* measOutput, sMeasConfigParam* measConfigParam)
{
	uint8_t i_dot, j_dot, i_Cohe, i_meas, id_CoheOut = 0;
	float tmpMag_j0, tmpMag_j1;
	float range_tmp;
	uint8_t  cohesionFinish;
	uint8_t dotsNum = 0;
	uint32_t tmpframeId = 0;
	int16_t leftBoundaryAziRad;
	int16_t curAziRad;
	sTwsDotCohesion* TwsDotCohesion = TwsDotCoheStr->TwsDotCohesion;
	sDots_Cohesion_vector* dotCluster;
	uint32_t* frameId;
	uint8_t dotCoheNum = TwsDotCoheStr->dotCoheNum;//存储区已有目标簇总数
	sDots_Cohesion_vector* tmpDots_Cohesion = gDots_Cohesion_vector;//目标簇互换的临时结构体


		//** 凝聚完输出结构体初始化 **//
	memset(measOutput, 0, sizeof(sMeasOutput));

	if (dotCoheNum > 0)
	{
		////** 凝聚完输出结构体初始化 **//
		//memset(measOutput, 0, sizeof(sMeasOutput));

		for (i_Cohe = 0; i_Cohe < dotCoheNum; i_Cohe++)
		{
			dotsNum = TwsDotCohesion[i_Cohe].dotsNum; //已有点迹总数
			cohesionFinish = TwsDotCohesion[i_Cohe].cohesionFinish;//是否凝聚完成标志

			//****** 目标簇是否满足测角条件判断 ******//
			leftBoundaryAziRad = TwsDotCohesion[i_Cohe].leftBoundaryAziRad;//扫描波束左边界方位角
			curAziRad = measInput->curAziBeamRad;//measInput->dotInput[0].aziBeamRad;//当前扫描波束方位角

			range_tmp = TwsDotCohesion[i_Cohe].dotCluster[0].range + measConfigParam->rCohGatingTws;
			TwsDotCohesion[i_Cohe].cohesionFinish = 0U;
			if (range_tmp < RANGE_THRESHOLD)
			{
				//单波位+当前波位扫描到最边界位置(一轮扫描结束)
				if ((fabsf(curAziRad - leftBoundaryAziRad) > AZIMUTH_GAP_BEAM_3) || (measInput->wholeSpaceScanCycleFinalBeamFlag == 1))
				{
					TwsDotCohesion[i_Cohe].cohesionFinish = 1U;
				}
			}
			else
			{
				//目标簇聚类点迹超过k个方位波位未更新+单波位+当前波位扫描到最边界位置(一轮扫描结束)
				if ((fabsf(curAziRad - leftBoundaryAziRad) > AZIMUTH_GAP_BEAM_2) || (measInput->wholeSpaceScanCycleFinalBeamFlag == 1))//(cohesionFinish == 0) && 
				{
					TwsDotCohesion[i_Cohe].cohesionFinish = 1U;
				}
			}

			if ( 1U == TwsDotCohesion[i_Cohe].cohesionFinish )
			{
				dotCluster = TwsDotCohesion[i_Cohe].dotCluster;
				frameId = TwsDotCohesion[i_Cohe].frameId;
				//****** (1)按照mag幅值进行降序冒泡排序 *****//
				for (i_dot = 0; i_dot < dotsNum - 1; i_dot++)
				{
					for (j_dot = 0; j_dot < dotsNum - i_dot - 1; j_dot++)
					{
						tmpMag_j0 = dotCluster[j_dot].mag;
						tmpMag_j1 = dotCluster[j_dot + 1].mag;

						memset(tmpDots_Cohesion, 0, sizeof(sDots_Cohesion_vector));
						if (tmpMag_j0 < tmpMag_j1)
						{
							//目标簇互换
							memcpy(tmpDots_Cohesion, &dotCluster[j_dot], sizeof(sDots_Cohesion_vector));
							memcpy(&dotCluster[j_dot], &dotCluster[j_dot + 1], sizeof(sDots_Cohesion_vector));
							memcpy(&dotCluster[j_dot + 1], tmpDots_Cohesion, sizeof(sDots_Cohesion_vector));

							//帧ID互换
							tmpframeId = frameId[j_dot];
							frameId[j_dot] = frameId[j_dot + 1];
							frameId[j_dot + 1] = tmpframeId;
						}
					}
				}

				//****** (2)按照mag幅值进行加权凝聚+测角 *****//
				dotsCohesionFun_tws(measInput, i_Cohe, TwsDotCohesion, id_CoheOut, measOutput);
				id_CoheOut++;//凝聚输出ID累加
			}
		}
		//输出结构体状态参数赋值
		if (id_CoheOut > 0)
		{
			measOutput->num = id_CoheOut; //凝聚完的目标总数
			measOutput->condenceDone = 1;//输出的目标均已凝聚完
			measOutput->scanType = measInput->scanType;//波束扫描模式，TWS/TAS
			measOutput->tasTargetId = -1;//TWS模式TAS跟踪目标ID赋值-1
			measOutput->timestamp = measInput->timestamp;//时间戳
			measOutput->aziBeamSin = (int16_t)(sinf(measInput->dotInput[0].aziBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位方位角
			measOutput->eleBeamSin = (int16_t)(sinf(measInput->dotInput[0].eleBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位俯仰角
		}
	}

	//*** 打印输出信息 ***//
	if (measOutput->num > 0)
	{
		for (i_meas = 0; i_meas < measOutput->num; i_meas++)
		{
#ifdef DEBUG_LOG_HXJ
			my_printf("444444444-TWS-measOut-444444444 frameID[ %d ], curAziBeamRad[ %d ], measNum[ %d ], measId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], detNum[ %d ], detectionId[10][ %d,%d,%d,%d,%d,%d,%d,%d,%d,%d ], frameId[10][ %d,%d,%d,%d,%d,%d,%d,%d,%d,%d ],ifCalcAzi[ %d ],ifCalcEle[ %d ],aziRad[ %.5f,%.5f ],eleRad[ %.5f,%.5f ],mag[ %.5f,%.5f,%.5f,%.5f ],RCS[ %.5f ]\n", \
				measInput->frameID, measInput->curAziBeamRad, measOutput->num, measOutput->measurement[i_meas].detId, measOutput->measurement[i_meas].vector.range, measOutput->measurement[i_meas].vector.doppler, measOutput->measurement[i_meas].vector.azimuthRad, \
				measOutput->measurement[i_meas].vector.pitchRad, measOutput->measurement[i_meas].mag, measOutput->pointsIdInfo[i_meas].detectionNum, measOutput->pointsIdInfo[i_meas].detectionId[0], measOutput->pointsIdInfo[i_meas].detectionId[1], \
				measOutput->pointsIdInfo[i_meas].detectionId[2], measOutput->pointsIdInfo[i_meas].detectionId[3], measOutput->pointsIdInfo[i_meas].detectionId[4], measOutput->pointsIdInfo[i_meas].detectionId[5], measOutput->pointsIdInfo[i_meas].detectionId[6], \
				measOutput->pointsIdInfo[i_meas].detectionId[7], measOutput->pointsIdInfo[i_meas].detectionId[8], measOutput->pointsIdInfo[i_meas].detectionId[9], measOutput->pointsIdInfo[i_meas].frameId[0], \
				measOutput->pointsIdInfo[i_meas].frameId[1], measOutput->pointsIdInfo[i_meas].frameId[2], measOutput->pointsIdInfo[i_meas].frameId[3], measOutput->pointsIdInfo[i_meas].frameId[4], measOutput->pointsIdInfo[i_meas].frameId[5], \
				measOutput->pointsIdInfo[i_meas].frameId[6], measOutput->pointsIdInfo[i_meas].frameId[7], measOutput->pointsIdInfo[i_meas].frameId[8], measOutput->pointsIdInfo[i_meas].frameId[9], measOutput->calcAngleInfo[i_meas].ifCalcAzi, measOutput->calcAngleInfo[i_meas].ifCalcEle, \
				measOutput->calcAngleInfo[i_meas].aziBeamRadList[0], measOutput->calcAngleInfo[i_meas].aziBeamRadList[1], measOutput->calcAngleInfo[i_meas].eleBeamRadList[0], measOutput->calcAngleInfo[i_meas].eleBeamRadList[1], measOutput->calcAngleInfo[i_meas].MagBeamList[0],
				measOutput->calcAngleInfo[i_meas].MagBeamList[1], measOutput->calcAngleInfo[i_meas].MagBeamList[2], measOutput->calcAngleInfo[i_meas].MagBeamList[3], measOutput->measurement[i_meas].rcs);
#endif // DEBUG_LOG_HXJ
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ
		my_printf("444444440-TWS-measOut-444444440 frameID[ %d ], measNum[ %d ], curAziBeamRad[ %d ]\n", measInput->frameID, measOutput->num, measInput->curAziBeamRad);
#endif // DEBUG_LOG_HXJ
	}

}

//**** TWS模式-(4-1-1)按照mag幅值进行加权凝聚+测角 ****//
void dotsCohesionFun_tws(sMeasInput* measInput, uint8_t i_Cohe, sTwsDotCohesion* TwsDotCohesion, uint8_t id_CoheOut, sMeasOutput* measOutput)
{
	uint8_t i_dot, j_dot;
	uint8_t ifCalcAzi = 0, ifCalcEle = 0, CalcEleFlag = 0;
	float tmpMag, sumMag, tmpRange, tmpDoppler;
	float K, Mag, Range, Doppler;
	float mag1, mag2, maxSumMag_azi, maxSumMag_ele;
	int16_t aziBeamRad1, eleBeamRad1, aziBeamRad2, eleBeamRad2;
	float azimuthRad, elevationRad, differSumRatio_azi, differSumRatio_ele;
	float k_azi_tws, k_ele_tws;
	float diffSumThd_azi_tws_L, diffSumThd_azi_tws_R, diffSumThd_ele_tws_L, diffSumThd_ele_tws_R;
	float MagBeamList[COCH_BEAM_NUM_TWS] = { 0.0f };
	float azimuthRadList[COCH_BEAM_NUM_TWS / 2] = { 0.0f };
	float eleBeamRadList[COCH_BEAM_NUM_TWS / 2] = { 0.0f };
	sMeasurement_SphPoint_Mid measurement_mid = {0.0f, 0, 0.0f, 0.0f};
	Cor_Reflector_RCS  Cor_Reflector_RCS_Parameter = { 80.8398f,10.f * log10(1.f), 204 };//RCS标定结果：mag，RCS，R
	//Cor_Reflector_RCS  Cor_Reflector_RCS_Parameter = { 84.8398f,10.f * log10(1.f), 102 };//RCS标定结果：mag，RCS，R
	//Cor_Reflector_RCS  Cor_Reflector_RCS_Parameter = { 72.475f,10.f * log10(1.f), 204 };//RCS标定结果：mag，RCS，R
	//Cor_Reflector_RCS  Cor_Reflector_RCS_Parameter = { 52.721f,10.f * log10(0.03f), 204 };//RCS标定结果：mag，RCS，R

	uint8_t dotsNum = TwsDotCohesion[i_Cohe].dotsNum;//聚类目标簇点迹总数
	uint32_t* frameId = TwsDotCohesion[i_Cohe].frameId;//聚类目标簇点迹帧号数组
	sDots_Cohesion_vector* dotCluster = TwsDotCohesion[i_Cohe].dotCluster;//聚类目标簇点迹缓存结构体
	sMeasurement_SphPoint* measurement = measOutput->measurement;//凝聚目标簇输出结构体
	sDotCohPointsIdInfo* pointsIdInfo = measOutput->pointsIdInfo;//凝聚目标簇聚类点迹信息
	sCalcAngleInfo* calcAngleInfo = measOutput->calcAngleInfo;//凝聚目标簇测角相关信息
	int iMeas = 0;

	Mag = 0;
	Range = 0;
	Doppler = 0;
	sumMag = 0;

	//***** (1)点迹幅值总和计算 *****//
	for (i_dot = 0; i_dot < dotsNum; i_dot++)
	{
		tmpMag = dotCluster[i_dot].mag;//幅值
		sumMag = sumMag + tmpMag;//幅值总和
	}

	//***** (2)聚类目标range ,doppler mag计算 *****//
	for (i_dot = 0; i_dot < dotsNum; i_dot++)
	{
		tmpMag = dotCluster[i_dot].mag;//幅值
		tmpRange = dotCluster[i_dot].range;// * INV_ONE6FORMAT_SCALE;
		tmpDoppler = dotCluster[i_dot].doppler;// * INV_ONE6FORMAT_SCALE;

		K = tmpMag / sumMag;
		Mag = Mag + tmpMag * K;
		Range = Range + tmpRange * K;
		Doppler = Doppler + tmpDoppler * K;

		//***** (3)凝聚目标簇聚类点迹信息赋值 *****//
		pointsIdInfo[id_CoheOut].detectionId[i_dot] = dotCluster[i_dot].dotId;
		pointsIdInfo[id_CoheOut].frameId[i_dot] = frameId[i_dot];
	}
	pointsIdInfo[id_CoheOut].detectionNum = dotsNum;//凝聚目标簇聚类点迹总数

   //***** (4)比幅测角点迹选取 *****//
	azimuthRadList[0] = dotCluster[0].azimuthRad;
	eleBeamRadList[0] = dotCluster[0].elevationRad;
	/*eleBeamRadList[0] = (float)(dotCluster[0].eleBeamRad * INV_ONE13FORMAT_SCALE);*/

	MagBeamList[0] = powf(10.f, dotCluster[0].mag / 10.f);//mag1
	MagBeamList[2] = powf(10.f, dotCluster[0].mag / 10.f);//mag2

	maxSumMag_azi = 0;
	maxSumMag_ele = 0;
	for (i_dot = 0; i_dot < dotsNum - 1; i_dot++)
	{
		aziBeamRad1 = dotCluster[i_dot].aziBeamRad;
		eleBeamRad1 = (int16_t)(dotCluster[i_dot].elevationRad * ONE13FORMAT_SCALE);
		/*eleBeamRad1 = dotCluster[i_dot].eleBeamRad;*/
		mag1 = dotCluster[i_dot].mag;

		for (j_dot = i_dot + 1; j_dot < dotsNum; j_dot++)
		{
			aziBeamRad2 = dotCluster[j_dot].aziBeamRad;
			eleBeamRad2 = (int16_t)(dotCluster[j_dot].elevationRad * ONE13FORMAT_SCALE);
			//eleBeamRad2 = dotCluster[j_dot].eleBeamRad;
			mag2 = dotCluster[j_dot].mag;

			//满足测方位角条件
			//if ((aziBeamRad1 != aziBeamRad2) && (eleBeamRad1 == eleBeamRad2) && (mag1 + mag2 > maxSumMag_azi))
			if ((aziBeamRad1 != aziBeamRad2) && (fabsf(eleBeamRad1 - eleBeamRad2) <= 0.05 * DEG2RAD) && (mag1 + mag2 > maxSumMag_azi))
			{
				//azimuth of sum-difference ratio amplitude 
				azimuthRadList[0] = dotCluster[i_dot].azimuthRad;
				azimuthRadList[1] = dotCluster[j_dot].azimuthRad;

				MagBeamList[0] = powf(10.f, mag1 / 10.f);//mag1
				MagBeamList[1] = powf(10.f, mag2 / 10.f);//mag2

				//if (eleBeamRad1 == eleBeamRad2)
				//if ((fabsf(aziBeamRad1 - aziBeamRad2) <= AZIMUTH_GAP_BEAM_1) && (eleBeamRad1 == eleBeamRad2))
				if ((fabsf(aziBeamRad1 - aziBeamRad2) <= AZIMUTH_GAP_BEAM_1))
				{
					ifCalcAzi = 1;
				}

				maxSumMag_azi = mag1 + mag2;
			}

			//满足测俯仰角条件
			//if ((aziBeamRad1 == aziBeamRad2) && (eleBeamRad1 != eleBeamRad2) && (mag1 + mag2 > maxSumMag_ele))
			if ((eleBeamRad1 != eleBeamRad2) && (fabsf(aziBeamRad1 - aziBeamRad2) <= 0.05 * DEG2RAD) &&  (mag1 + mag2 > maxSumMag_ele))
			{
				/*eleBeamRadList[0] = (float)(eleBeamRad1 * INV_ONE13FORMAT_SCALE);
				eleBeamRadList[1] = (float)(eleBeamRad2 * INV_ONE13FORMAT_SCALE);*/
				eleBeamRadList[0] = dotCluster[i_dot].elevationRad;
				eleBeamRadList[1] = dotCluster[j_dot].elevationRad;


				MagBeamList[2] = powf(10.f, mag1 / 10.f);//mag1
				MagBeamList[3] = powf(10.f, mag2 / 10.f);//mag2

				//if (aziBeamRad1 == aziBeamRad2)
				//if ((fabsf(eleBeamRad1 - eleBeamRad2) <= ELEVATION_GAP_BEAM) && (aziBeamRad1 == aziBeamRad2))
				if ((fabsf(eleBeamRad1 - eleBeamRad2) <= ELEVATION_GAP_BEAM))
				{
					ifCalcEle = 1;
				}

				maxSumMag_ele = mag1 + mag2;
			}
		}
	}

	//***** (5)比幅测角 *****//
	//凝聚后目标先赋值目标簇中幅值最大点迹的波位角，不满足做加权测角！！！！！111111
	azimuthRad = dotCluster[0].azimuthRad;
	elevationRad = dotCluster[0].elevationRad;
	/*elevationRad = (float)(dotCluster[0].eleBeamRad * INV_ONE13FORMAT_SCALE);*/
	CalcEleFlag = 0;//单点输出幅值最大波位角标记

	measurement_mid.azimuthRad = azimuthRad;
	measurement_mid.elevationRad = elevationRad;

	//测角参数区分2代和3代雷达
	if (3 == g_byDevHwGen)
	{
		/*k_ele_tws = K_ELE_TWS_3;
		diffSumThd_ele_tws_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS_3;
		diffSumThd_ele_tws_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS_3;*/

		k_ele_tws = K_ELE_TWS_4;
		diffSumThd_ele_tws_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS_4;
		diffSumThd_ele_tws_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS_4;
	}
	else 
	{
		k_ele_tws = K_ELE_TWS;
		diffSumThd_ele_tws_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS;
		diffSumThd_ele_tws_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS;
	}


	//满足条件，进入测角处理，更新对应角度值
	if ((MagBeamList[0] != 0) && (MagBeamList[1] != 0))
	{
		if ((azimuthRadList[0] <= azimuthRadList[1]))
		{
			measurement_mid.azimuthRad = azimuthRadList[0];
		}
		else
		{
			measurement_mid.azimuthRad = azimuthRadList[1];
		}
		//(1)检测完成和差测角，凝聚仅做幅值占比加权测角
		azimuthRad = (MagBeamList[0] / (MagBeamList[0] + MagBeamList[1])) * azimuthRadList[0] + (MagBeamList[1] / (MagBeamList[0] + MagBeamList[1])) * azimuthRadList[1];

	}
	if ((MagBeamList[2] != 0) && (MagBeamList[3] != 0))//测俯仰角
	{

		if (eleBeamRadList[0] <= eleBeamRadList[1])
		{
			measurement_mid.elevationRad = eleBeamRadList[0];
		}
		else
		{
			measurement_mid.elevationRad = eleBeamRadList[1];
		}
		//(1)幅值占比加权测角
		// elevationRad = (MagBeamList[2] / (MagBeamList[2] + MagBeamList[3])) * eleBeamRadList[0] + (MagBeamList[3] / (MagBeamList[2] + MagBeamList[3])) * eleBeamRadList[1];

		//(1.1)幅值最大点波位角
		if (MagBeamList[2] >= MagBeamList[3])
		{
			elevationRad = eleBeamRadList[0];
		}		
		else
		{
			elevationRad = eleBeamRadList[1];
		}

		CalcEleFlag = 1;//加权测角标记


		//(2)比幅测角，满足条件做更新
		if (ifCalcEle == 1)
		{
			differSumRatio_ele = (MagBeamList[2] - MagBeamList[3]) / (MagBeamList[2] + MagBeamList[3]);

			if ((eleBeamRadList[0] <= eleBeamRadList[1]) && (differSumRatio_ele >= diffSumThd_ele_tws_L) && (differSumRatio_ele <= diffSumThd_ele_tws_R))
			{          
				//elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tws * cosf(eleBeamRadList[1])) * differSumRatio_ele;//phiL = phi_Cur     
				elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tws) * differSumRatio_ele;//phiL = phi_Cur,(k_ele_tws * cosf(eleBeamRadList[1]))
				CalcEleFlag = 2;//比幅测角标记
			}
			if ((eleBeamRadList[0] > eleBeamRadList[1]) && (-differSumRatio_ele >= diffSumThd_ele_tws_L) && (-differSumRatio_ele <= diffSumThd_ele_tws_R))
			{
				//elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tws * cosf(eleBeamRadList[0])) * (-differSumRatio_ele);//phiL = phi_Cache
				elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tws) * (-differSumRatio_ele);//phiL = phi_Cache,(k_ele_tws * cosf(eleBeamRadList[0]))
				CalcEleFlag = 2;//比幅测角标记
			}
		}

	}
	
	//***** (6)完成凝聚后的目标赋值 *****//
	measurement[id_CoheOut].detId = id_CoheOut;
	measurement[id_CoheOut].mag = Mag;
	measurement[id_CoheOut].vector.range = Range;
	if (gVelunambigType == STEP_SCAN)
	{
		if (TwsDotCohesion[i_Cohe].unambigDone)
		{
			measurement[id_CoheOut].vector.doppler = TwsDotCohesion[i_Cohe].unambigVel;
		}
		else
		{
			measurement[id_CoheOut].vector.doppler = Doppler;
		}
	}
	else
	{
		measurement[id_CoheOut].vector.doppler = Doppler;
	}
	measurement[id_CoheOut].unambigVelDone = TwsDotCohesion[i_Cohe].unambigDone;

	measurement[id_CoheOut].unambigVel= TwsDotCohesion[i_Cohe].unambigVel;
	measurement[id_CoheOut].vector.azimuthRad = azimuthRad;
	measurement[id_CoheOut].vector.pitchRad = elevationRad;
	measurement[id_CoheOut].x = Range * cosf(elevationRad) * cosf(azimuthRad);
	measurement[id_CoheOut].y = Range * cosf(elevationRad) * sinf(azimuthRad);
	measurement[id_CoheOut].z = Range * sinf(elevationRad);
	measurement[id_CoheOut].sinAzim = sinf(azimuthRad);
	measurement[id_CoheOut].cosAzim = cosf(azimuthRad);
	//measurement[id_CoheOut].aziBeamSin = (int16_t)(sinf(measInput->dotInput[0].aziBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位方位角
	//measurement[id_CoheOut].eleBeamSin = (int16_t)(sinf(measInput->dotInput[0].eleBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位俯仰角
	if (!measurement[id_CoheOut].unambigVelDone) {
		measurement[id_CoheOut].waveType = dotCluster[0].waveType;
	}
#ifdef DEBUG_UNAMBIVEL
	my_printf("measId %d, unambiDone %d, waveType %d, vel %.2f", id_CoheOut, measurement[id_CoheOut].unambigVelDone, \
		measurement[id_CoheOut].waveType, measurement[id_CoheOut].unambigVel);
	for (iMeas = 0; iMeas < TwsDotCohesion[i_Cohe].dotsNum; iMeas++)
	{
		my_printf("frameid %d detId %d wtype %d vel %.2f", TwsDotCohesion[i_Cohe].frameId[iMeas], \
			TwsDotCohesion[i_Cohe].dotCluster[iMeas].dotId, dotCluster[iMeas].waveType, dotCluster[iMeas].doppler);
	}
#endif // DEBUG_UNAMBIVEL

	measurement[id_CoheOut].classification = 0; //TODO: input real value

	measurement_mid.mag = Mag;
	measurement_mid.range = Range;
	RCS_Estimation(&measurement_mid, &Cor_Reflector_RCS_Parameter, measurement + id_CoheOut, Gain_IF);

	calcAngleInfo[id_CoheOut].ifCalcAzi = ifCalcAzi;
	calcAngleInfo[id_CoheOut].ifCalcEle = CalcEleFlag;//俯仰测角标记：0单点波位角，1加权测角，2比幅测角
	memcpy(calcAngleInfo[id_CoheOut].MagBeamList, &MagBeamList[0], COCH_BEAM_NUM_TWS * sizeof(float));
	memcpy(calcAngleInfo[id_CoheOut].azimuthRadList, &azimuthRadList[0], (COCH_BEAM_NUM_TWS / 2) * sizeof(float));
	memcpy(calcAngleInfo[id_CoheOut].eleBeamRadList, &eleBeamRadList[0], (COCH_BEAM_NUM_TWS / 2) * sizeof(float));

}

void RCS_Estimation(sMeasurement_SphPoint_Mid* detect, Cor_Reflector_RCS* Cor_RCS, sMeasurement_SphPoint* decodedBits, float* Gain_IF)
{
	float sigma_est = 0.0f;
	float azi_y = 0.0f;
	float ele_y = 0.0f;
	float azi_a = -0.258f;
	float azi_b = 0.0054f;
	float azi_c = 0.0038f;
	float ele_a = -0.052f;
	float ele_b = -0.0176f;
	float ele_c =0.0103f; 	
	float azi_left = decodedBits->vector.azimuthRad - detect->azimuthRad;
	float azi_right = decodedBits->vector.azimuthRad - (detect->azimuthRad + 4.f * DEG2RAD);
	float ele_left = decodedBits->vector.pitchRad - detect->elevationRad;
	float ele_right = decodedBits->vector.pitchRad - (detect->elevationRad + 6.f * DEG2RAD);
	if (fabsf(azi_left) <  fabsf(azi_right))
	{
		azi_y =(float)(azi_a * powf(fabsf(azi_left * RAD2DEG),2.f) + azi_b * fabsf(azi_left * RAD2DEG) + azi_c);
	}
	else{
		azi_y = (float)(azi_a * powf(fabsf(azi_right * RAD2DEG), 2.f) + azi_b * fabsf(azi_right * RAD2DEG) + azi_c);
	}
	if (fabsf(ele_left) < fabsf(ele_right))
	{
		ele_y = (float)(ele_a * powf(fabsf(ele_left * RAD2DEG), 2.f) + ele_b * fabsf(ele_left * RAD2DEG)+ ele_c);
	}
	else{
		ele_y = (float)(ele_a * powf(fabsf(ele_right * RAD2DEG), 2.f) + ele_b * fabsf(ele_right * RAD2DEG) + ele_c);
	}
	sigma_est = ( detect->mag - Gain_IF[(int)(detect->range - 1)] ) - ( Cor_RCS->Mag0 - Gain_IF[Cor_RCS->R0 - 1] )+ Cor_RCS->RCS0   \
				 + 18.f * ( log10f((float)detect->range) - log10f((float)Cor_RCS->R0) ) \
				 - 20.f * log10f(cosf(detect->azimuthRad) * cosf(detect->elevationRad)) + fabsf(azi_y) + fabsf(ele_y);
	//sigma_est = detect->mag - 20.f * log10f(cosf(detect->azimuthRad) * cosf(detect->elevationRad)) - Gain_IF[detect->range] \
	//	-Cor_RCS->Mag0 + Cor_RCS -> RCS0 + 18.f * log10f((float)detect->range) - 18.f * log10f((float)Cor_RCS->R0) + Gain_IF[Cor_RCS->R0]\
	//	+ fabsf(azi_y) + fabsf(ele_y);
	decodedBits->rcs  = 10.f * powf(10.f,sigma_est / 10.f);
}


//**** TAS模式-(4.2)判断目标簇是否满足测角条件，更新cohesionFinish标记+点迹凝聚+测角函数 ****//
void dotsCohesionAndOutput_tas(sMeasInput* measInput, sTasDotCohesion* TasDotCohesion, sMeasOutput* measOutput)
{
	uint8_t i_dot, j_dot, i_meas;
	float tmpMag_j0, tmpMag_j1;
	uint8_t dotsNum = 0;
	uint32_t tmpframeId = 0;
	sDots_Cohesion_vector* dotCluster;
	uint32_t* frameId;
	sDots_Cohesion_vector* tmpDots_Cohesion = (sDots_Cohesion_vector*)gDots_Cohesion_vector;//目标簇互换的临时结构体

	//** 凝聚完输出结构体初始化 **//
	memset(measOutput, 0, sizeof(sMeasOutput));
	dotsNum = TasDotCohesion[0].dotsNum; //已有点迹总数

	measOutput->scanType = measInput->scanType;//波束扫描模式，TWS/TAS
	measOutput->tasTargetId = measInput->tasTrackStr[0].tasTargetId;//TAS跟踪目标ID
	measOutput->timestamp = measInput->timestamp;//时间戳
	measOutput->aziBeamSin = (int16_t)(sinf(measInput->dotInput[0].aziBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位方位角
	measOutput->eleBeamSin = (int16_t)(sinf(measInput->dotInput[0].eleBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位俯仰角
	measOutput->measurement[0].waveType = TasDotCohesion->dotCluster[0].waveType;
	//****** 目标簇是否满足测角条件判断 ******//		
	if (measInput->tasOrInitTrackFinalBeamFlag == 1)//TAS目标设定波位扫描完成
	{
		TasDotCohesion[0].cohesionFinish = 1;
		measOutput->condenceDone = 1;
		if (dotsNum > 0)
		{
			dotCluster = TasDotCohesion[0].dotCluster;
			frameId = TasDotCohesion[0].frameId;

			//****** (1)按照mag幅值进行降序冒泡排序 *****//
			for (i_dot = 0; i_dot < dotsNum - 1; i_dot++)
			{
				for (j_dot = 0; j_dot < dotsNum - i_dot - 1; j_dot++)
				{
					tmpMag_j0 = dotCluster[j_dot].mag;
					tmpMag_j1 = dotCluster[j_dot + 1].mag;

					memset(tmpDots_Cohesion, 0, sizeof(sDots_Cohesion_vector));
					if (tmpMag_j0 < tmpMag_j1)
					{
						//目标簇互换
						memcpy(tmpDots_Cohesion, &dotCluster[j_dot], sizeof(sDots_Cohesion_vector));
						memcpy(&dotCluster[j_dot], &dotCluster[j_dot + 1], sizeof(sDots_Cohesion_vector));
						memcpy(&dotCluster[j_dot + 1], tmpDots_Cohesion, sizeof(sDots_Cohesion_vector));

						//帧ID互换
						tmpframeId = frameId[j_dot];
						frameId[j_dot] = frameId[j_dot + 1];
						frameId[j_dot + 1] = tmpframeId;
					}
				}
			}

			//****** (2)按照mag幅值进行加权凝聚+测角，ID=0,仅TAS目标一个 *****//
			dotsCohesionFun_tas(measInput, TasDotCohesion, measOutput);

			//输出结构体状态参数赋值
			measOutput->num = 1; //凝聚完的目标总数
		}
	}

	//*** 打印输出信息 ***//
	if (measOutput->num > 0)
	{
		for (i_meas = 0; i_meas < measOutput->num; i_meas++)
		{
#ifdef DEBUG_LOG_HXJ
			my_printf("444444444-TAS-measOut-444444444 frameID[ %d ], curAziBeamRad[ %d ], measNum[ %d ], measId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], detNum[ %d ], detectionId[10][ %d,%d,%d,%d,%d,%d,%d,%d,%d,%d ], frameId[10][ %d,%d,%d,%d,%d,%d,%d,%d,%d,%d ],ifCalcAzi[ %d ],\
ifCalcEle[ %d ],aziRad[ %.5f,%.5f ],eleRad[ %.5f,%.5f ],mag[ %.5f,%.5f,%.5f,%.5f ]\n", \
				measInput->frameID, measInput->curAziBeamRad, measOutput->num, measOutput->measurement[i_meas].detId, measOutput->measurement[i_meas].vector.range, measOutput->measurement[i_meas].vector.doppler, measOutput->measurement[i_meas].vector.azimuthRad, \
				measOutput->measurement[i_meas].vector.pitchRad, measOutput->measurement[i_meas].mag, measOutput->pointsIdInfo[i_meas].detectionNum, measOutput->pointsIdInfo[i_meas].detectionId[0], measOutput->pointsIdInfo[i_meas].detectionId[1], \
				measOutput->pointsIdInfo[i_meas].detectionId[2], measOutput->pointsIdInfo[i_meas].detectionId[3], measOutput->pointsIdInfo[i_meas].detectionId[4], measOutput->pointsIdInfo[i_meas].detectionId[5], measOutput->pointsIdInfo[i_meas].detectionId[6], \
				measOutput->pointsIdInfo[i_meas].detectionId[7], measOutput->pointsIdInfo[i_meas].detectionId[8], measOutput->pointsIdInfo[i_meas].detectionId[9], measOutput->pointsIdInfo[i_meas].frameId[0], \
				measOutput->pointsIdInfo[i_meas].frameId[1], measOutput->pointsIdInfo[i_meas].frameId[2], measOutput->pointsIdInfo[i_meas].frameId[3], measOutput->pointsIdInfo[i_meas].frameId[4], measOutput->pointsIdInfo[i_meas].frameId[5], \
				measOutput->pointsIdInfo[i_meas].frameId[6], measOutput->pointsIdInfo[i_meas].frameId[7], measOutput->pointsIdInfo[i_meas].frameId[8], measOutput->pointsIdInfo[i_meas].frameId[9], measOutput->calcAngleInfo[i_meas].ifCalcAzi, measOutput->calcAngleInfo[i_meas].ifCalcEle, \
				measOutput->calcAngleInfo[i_meas].aziBeamRadList[0], measOutput->calcAngleInfo[i_meas].aziBeamRadList[1], measOutput->calcAngleInfo[i_meas].eleBeamRadList[0], measOutput->calcAngleInfo[i_meas].eleBeamRadList[1], measOutput->calcAngleInfo[i_meas].MagBeamList[0],
				measOutput->calcAngleInfo[i_meas].MagBeamList[1], measOutput->calcAngleInfo[i_meas].MagBeamList[2], measOutput->calcAngleInfo[i_meas].MagBeamList[3]);
#endif // DEBUG_LOG_HXJ
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ
		my_printf("444444440-TAS-measOut-444444440 frameID[ %d ], measNum[ %d ], curAziBeamRad[ %d ]\n", measInput->frameID, measOutput->num, measInput->curAziBeamRad);
#endif // DEBUG_LOG_HXJ
	}

}

//**** TAS模式-(4-2-1)按照mag幅值进行加权凝聚+测角 ****//
void dotsCohesionFun_tas(sMeasInput* measInput, sTasDotCohesion* TasDotCohesion, sMeasOutput* measOutput)
{
	uint8_t i_dot, j_dot;
	uint8_t ifCalcAzi = 0, ifCalcEle = 0, CalcEleFlag = 0;
	float tmpMag, sumMag, tmpRange, tmpDoppler;
	float K, Mag, Range, Doppler;
	float mag1, mag2, maxSumMag_azi, maxSumMag_ele;
	int16_t aziBeamRad1, eleBeamRad1, aziBeamRad2, eleBeamRad2;
	float azimuthRad, elevationRad, differSumRatio_azi, differSumRatio_ele;
	float k_azi_tas, k_ele_tas;
	float diffSumThd_azi_tas_L, diffSumThd_azi_tas_R, diffSumThd_ele_tas_L, diffSumThd_ele_tas_R;
	float MagBeamList[COCH_BEAM_NUM_TWS] = { 0.0f };
	float azimuthRadList[COCH_BEAM_NUM_TWS / 2] = { 0.0f };
	float eleBeamRadList[COCH_BEAM_NUM_TWS / 2] = { 0.0f };
	uint8_t Idx_fov_tas = 4U;
	float Idx_fov_tas_float = 0.f;


	uint8_t dotsNum = TasDotCohesion[0].dotsNum;//聚类目标簇点迹总数
	uint32_t* frameId = (uint32_t*)TasDotCohesion[0].frameId;//聚类目标簇点迹帧号数组
	sDots_Cohesion_vector* dotCluster = (sDots_Cohesion_vector*)TasDotCohesion[0].dotCluster;//聚类目标簇点迹缓存结构体
	sMeasurement_SphPoint* measurement = (sMeasurement_SphPoint*)measOutput->measurement;//凝聚目标簇输出结构体
	sDotCohPointsIdInfo* pointsIdInfo = (sDotCohPointsIdInfo*)measOutput->pointsIdInfo;//凝聚目标簇聚类点迹信息
	sCalcAngleInfo* calcAngleInfo = measOutput->calcAngleInfo;//凝聚目标簇测角相关信息


	Mag = 0;
	Range = 0;
	Doppler = 0;
	sumMag = 0;

	//***** (1)点迹幅值总和计算 *****//
	for (i_dot = 0; i_dot < dotsNum; i_dot++)
	{
		tmpMag = dotCluster[i_dot].mag;//幅值
		sumMag = sumMag + tmpMag;//幅值总和
	}

	//***** (2)聚类目标range ,doppler mag计算 *****//
	for (i_dot = 0; i_dot < dotsNum; i_dot++)
	{
		tmpMag = dotCluster[i_dot].mag;//幅值
		tmpRange = dotCluster[i_dot].range;// * INV_ONE6FORMAT_SCALE;
		tmpDoppler = dotCluster[i_dot].doppler;// * INV_ONE6FORMAT_SCALE;

		K = tmpMag / sumMag;
		Mag = Mag + tmpMag * K;
		Range = Range + tmpRange * K;
		Doppler = Doppler + tmpDoppler * K;

		//***** (3)凝聚目标簇聚类点迹信息赋值 *****//
		pointsIdInfo[0].detectionId[i_dot] = dotCluster[i_dot].dotId;
		pointsIdInfo[0].frameId[i_dot] = frameId[i_dot];
	}
	pointsIdInfo[0].detectionNum = dotsNum;//凝聚目标簇聚类点迹总数

   //***** (4)比幅测角点迹选取 *****//
	azimuthRadList[0] = dotCluster[0].azimuthRad;
	eleBeamRadList[0] = dotCluster[0].elevationRad;
	/*eleBeamRadList[0] = (float)(dotCluster[0].eleBeamRad * INV_ONE13FORMAT_SCALE);*/

	MagBeamList[0] = powf(10.f, dotCluster[0].mag / 10.f);//mag1
	MagBeamList[2] = powf(10.f, dotCluster[0].mag / 10.f);//mag2

	maxSumMag_azi = 0;
	maxSumMag_ele = 0;
	for (i_dot = 0; i_dot < dotsNum - 1; i_dot++)
	{
		aziBeamRad1 = dotCluster[i_dot].aziBeamRad;
		eleBeamRad1 = (int16_t)(dotCluster[i_dot].elevationRad * ONE13FORMAT_SCALE);
		/*eleBeamRad1 = dotCluster[i_dot].eleBeamRad;*/
		mag1 = dotCluster[i_dot].mag;

		for (j_dot = i_dot + 1; j_dot < dotsNum; j_dot++)
		{
			aziBeamRad2 = dotCluster[j_dot].aziBeamRad;
			eleBeamRad2 = (int16_t)(dotCluster[j_dot].elevationRad * ONE13FORMAT_SCALE);
			/*eleBeamRad2 = dotCluster[j_dot].eleBeamRad;*/
			mag2 = dotCluster[j_dot].mag;

			//满足测方位角条件
			//if ((aziBeamRad1 != aziBeamRad2) && (eleBeamRad1 == eleBeamRad2) && (mag1 + mag2 > maxSumMag_azi))
			if ((aziBeamRad1 != aziBeamRad2) && (fabsf(eleBeamRad1 - eleBeamRad2) <= 0.05 * DEG2RAD) && (mag1 + mag2 > maxSumMag_azi))
			{
				azimuthRadList[0] = dotCluster[i_dot].azimuthRad;
				azimuthRadList[1] = dotCluster[j_dot].azimuthRad;

				MagBeamList[0] = powf(10.f, mag1 / 10.f);//mag1
				MagBeamList[1] = powf(10.f, mag2 / 10.f);//mag2

				if ((fabsf(aziBeamRad1 - aziBeamRad2) <= AZIMUTH_GAP_BEAM_1))
				{
					ifCalcAzi = 1;
				}

				maxSumMag_azi = mag1 + mag2;
			}

			//满足测俯仰角条件
			//if ((eleBeamRad1 != eleBeamRad2) && (aziBeamRad1 == aziBeamRad2) && (mag1 + mag2 > maxSumMag_ele))
			if ((eleBeamRad1 != eleBeamRad2) && (fabsf(aziBeamRad1 - aziBeamRad2) <= 0.05*DEG2RAD) && (mag1 + mag2 > maxSumMag_ele))
			{
				/*eleBeamRadList[0] = (float)(eleBeamRad1 * INV_ONE13FORMAT_SCALE);
				eleBeamRadList[1] = (float)(eleBeamRad2 * INV_ONE13FORMAT_SCALE);*/
				eleBeamRadList[0] = dotCluster[i_dot].elevationRad;
				eleBeamRadList[1] = dotCluster[j_dot].elevationRad;

				MagBeamList[2] = powf(10.f, mag1 / 10.f);//mag1
				MagBeamList[3] = powf(10.f, mag2 / 10.f);//mag2

				if ((fabsf(eleBeamRad1 - eleBeamRad2) <= ELEVATION_GAP_BEAM))
				{
					ifCalcEle = 1;
				}

				maxSumMag_ele = mag1 + mag2;
			}
		}
	}

	//***** (5)比幅测角 *****//
	//凝聚后目标先赋值目标簇中幅值最大点迹的波位角
	azimuthRad = dotCluster[0].azimuthRad;
	elevationRad = dotCluster[0].elevationRad;
	/*elevationRad = (float)(dotCluster[0].eleBeamRad * INV_ONE13FORMAT_SCALE);*/
	CalcEleFlag = 0;//单点输出幅值最大波位角标记

	//测角参数区分2代和3代雷达
	if (3 == g_byDevHwGen)
	{
		/*k_ele_tas = K_ELE_TAS_3;
		diffSumThd_ele_tas_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS_3;
		diffSumThd_ele_tas_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS_3;*/

		k_ele_tas = K_ELE_TAS_4;
		diffSumThd_ele_tas_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS_4;
		diffSumThd_ele_tas_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS_4;
	}
	else
	{
		k_ele_tas = K_ELE_TAS;
		diffSumThd_ele_tas_L = DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS;
		diffSumThd_ele_tas_R = DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS;
	}

	//满足条件，进入测角处理，更新对应角度值
	if ((MagBeamList[0] != 0) && (MagBeamList[1] != 0))
	{
		//(1)检测已完成和差测角，仅做幅值占比加权测角
		azimuthRad = (MagBeamList[0] / (MagBeamList[0] + MagBeamList[1])) * azimuthRadList[0] + (MagBeamList[1] / (MagBeamList[0] + MagBeamList[1])) * azimuthRadList[1];

	}
	if ((MagBeamList[2] != 0) && (MagBeamList[3] != 0))//测俯仰角
	{
		//(1)幅值占比加权测角
		//elevationRad = (MagBeamList[2] / (MagBeamList[2] + MagBeamList[3])) * eleBeamRadList[0] + (MagBeamList[3] / (MagBeamList[2] + MagBeamList[3])) * eleBeamRadList[1];
		
		//(1.1)幅值最大点波位角
		if (MagBeamList[2] >= MagBeamList[3])
		{
			elevationRad = eleBeamRadList[0];
		}		
		else
		{
			elevationRad = eleBeamRadList[1];
		}

		CalcEleFlag = 1;//加权测角标记

		//(2)比幅测角，满足条件做更新
		if (ifCalcEle == 1)
		{
			Idx_fov_tas_float = fabsf(eleBeamRadList[0] - eleBeamRadList[1]) * RAD2DEG;
			Idx_fov_tas = (uint8_t) floor(Idx_fov_tas_float + 0.5);
			if ( Idx_fov_tas < 1 )
			{
				Idx_fov_tas = 1;
			}
			else if  ( Idx_fov_tas > 8 )
			{
				Idx_fov_tas = 8;
			}
			k_ele_tas = K_Ele_TAS[Idx_fov_tas-1];
			diffSumThd_ele_tas_L = Ratio_Thrd_Lift_Ele_TAS[Idx_fov_tas-1];
			diffSumThd_ele_tas_R = Ratio_Thrd_Right_Ele_TAS[Idx_fov_tas-1];
			differSumRatio_ele = (MagBeamList[2] - MagBeamList[3]) / (MagBeamList[2] + MagBeamList[3]);

			if ((eleBeamRadList[0] <= eleBeamRadList[1]) && (differSumRatio_ele >= diffSumThd_ele_tas_L) && (differSumRatio_ele <= diffSumThd_ele_tas_R))
			{
				//elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tas * cosf(eleBeamRadList[1])) * differSumRatio_ele;//phiL = phi_Cur
				elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tas) * differSumRatio_ele;//phiL = phi_Cur,(k_ele_tas * cosf(eleBeamRadList[1]))
				CalcEleFlag = 2;//比幅测角标记
			}
			if ((eleBeamRadList[0] > eleBeamRadList[1]) && (-differSumRatio_ele >= diffSumThd_ele_tas_L) && (-differSumRatio_ele <= diffSumThd_ele_tas_R))
			{
				//elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tas * cosf(eleBeamRadList[0])) * (-differSumRatio_ele);//phiL = phi_Cache
				elevationRad = (eleBeamRadList[0] + eleBeamRadList[1]) / 2 + (k_ele_tas) * (-differSumRatio_ele);//phiL = phi_Cache, (k_ele_tas * cosf(eleBeamRadList[0]))
				CalcEleFlag = 2;//比幅测角标记
			}
		}

	}

	//***** (6)完成凝聚后的目标赋值 *****//
	measurement[0].detId = 0;
	measurement[0].mag = Mag;
	measurement[0].vector.range = Range;
	measurement[0].vector.doppler = Doppler;
	measurement[0].vector.azimuthRad = azimuthRad;
	measurement[0].vector.pitchRad = elevationRad;
	measurement[0].x = Range * cosf(elevationRad) * cosf(azimuthRad);
	measurement[0].y = Range * cosf(elevationRad) * sinf(azimuthRad);
	measurement[0].z = Range * sinf(elevationRad);
	measurement[0].sinAzim = sinf(azimuthRad);
	measurement[0].cosAzim = cosf(azimuthRad);
	//measurement[0].aziBeamSin = (int16_t)(sinf(measInput->dotInput[0].aziBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位方位角
	//measurement[0].eleBeamSin = (int16_t)(sinf(measInput->dotInput[0].eleBeamRad * INV_ONE13FORMAT_SCALE) * ONE15FORMAT_SCALE);//当前波位俯仰角
	//measurement[0].waveType = measInput->dotInput[0].waveType;//当前波位波形
	measurement[0].classification = 0; //TODO: input real value

	calcAngleInfo[0].ifCalcAzi = ifCalcAzi;
	calcAngleInfo[0].ifCalcEle = CalcEleFlag;//俯仰测角标记：0单点波位角，1加权测角，2比幅测角
	memcpy(calcAngleInfo[0].MagBeamList, &MagBeamList[0], COCH_BEAM_NUM_TWS * sizeof(float));
	memcpy(calcAngleInfo[0].azimuthRadList, &azimuthRadList[0], (COCH_BEAM_NUM_TWS / 2) * sizeof(float));
	memcpy(calcAngleInfo[0].eleBeamRadList, &eleBeamRadList[0], (COCH_BEAM_NUM_TWS / 2) * sizeof(float));
}




//** 5.Target Cluster Store Update **//
void tarClusterStoreUpdate(sMeasInput* measInput, sMeasProcessInst* measInst, sClusterDetect* clusterDet)
{
	if (measInput->scanType == TWS_SCAN)
	{
		tarClusterStoreUpdate_tws(measInput, clusterDet, measInst->TwsDotCoheStr);
	}
	else
	{
		tarClusterStoreUpdate_tas(measInput, clusterDet, measInst->TasDotCohesion);
	}
}

//**** TWS模式-(5.1)存储区目标簇更新+点迹属性清空 ****//
void tarClusterStoreUpdate_tws(sMeasInput* measInput, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_dot, i_det, i_Cohe, i_last, detNum, dotsNum_last;
	uint8_t dots_store_NumEnd = 0, dots_dots_NumEnd = 0, lonelyNumEnd = 0;
	uint8_t dots_store_NumRemain = 0, dots_dots_NumRemain = 0, lonelyNumRemain = 0;
	uint8_t dotClustId = 0, clustIdEnd = 0, clustIdEnd1 = 0, clustIdEnd2 = 0;
	uint8_t storeOutNum = 0, storeEmptyNum = 0;
	uint8_t cohesionFinish, cohesionFinish_last;
	uint8_t* clustID = clusterDet->clusterID;
	uint8_t dots_store_Num = clusterDet->dots_store_Num;//与存储区目标簇可聚类的目标总数
	uint8_t dots_dots_Num = clusterDet->dots_dots_Num;//点迹自身可聚类的目标总数
	uint8_t lonelyNum = clusterDet->lonelyNum;//孤点目标总数
	uint8_t dots_store_NumIn = clusterDet->dots_store_NumIn;//当前帧可聚类的目标--放入存储区的总数
	uint8_t dots_dots_NumIn = clusterDet->dots_dots_NumIn;//点迹自身可聚类的目标--放入存储区的总数
	uint8_t lonelyNumIn = clusterDet->lonelyNumIn;//孤点目标--放入存储区的总数

	sDots_Cohesion_vector* dotInput = (sDots_Cohesion_vector*)measInput->dotInput;
	sTwsDotCohesion* TwsDotCohesion = (sTwsDotCohesion*)TwsDotCoheStr->TwsDotCohesion;
	uint8_t dotCoheNum = TwsDotCoheStr->dotCoheNum;

	detNum = measInput->detNum;

	//(1)清除已凝聚输出目标簇空间，填充成末尾目标簇，并清空末尾目标簇空间
	if (dotCoheNum > 0)
	{
		for (i_Cohe = 0; i_Cohe < dotCoheNum; i_Cohe++)
		{
			cohesionFinish = TwsDotCohesion[i_Cohe].cohesionFinish;//是否凝聚完成标志

			if (cohesionFinish == 1)
			{
				//(1-1)清空已凝聚的目标簇存储区
				storeOutNum++;//清空目标簇个数累加
				memset(&TwsDotCohesion[i_Cohe], 0, sizeof(sTwsDotCohesion));

				//(1-2)将末尾目标簇存储区的目标簇替换到已凝聚的目标簇存储区,将末尾目标簇存储区清空
				for (i_last = dotCoheNum - 1; i_last > i_Cohe; i_last--)
				{
					dotsNum_last = TwsDotCohesion[i_last].dotsNum;
					cohesionFinish_last = TwsDotCohesion[i_last].cohesionFinish;
					if ((cohesionFinish_last == 0) && (dotsNum_last > 0))//末尾目标簇非空且不满足凝聚条件的往前移
					{
						memcpy(&TwsDotCohesion[i_Cohe], &TwsDotCohesion[i_last], sizeof(sTwsDotCohesion));
						TwsDotCohesion[i_Cohe].clusterId = i_Cohe;//更新存储区目标簇ID
						TwsDotCohesion[i_Cohe].cohesionFinish = 0;//更新是否凝聚完成标志为0

						memset(&TwsDotCohesion[i_last], 0, sizeof(sTwsDotCohesion));
						break;
					}
				}
			}
		}
	}

	dots_store_NumEnd = dotCoheNum - storeOutNum;//更新后的目标簇存储区已占用空间总数
	storeEmptyNum = TWS_DOT_COHESION_NUM - dots_store_NumEnd;//存储区空余目标簇空间个数  

	//(2)更新目标簇存储区
	if (detNum > 0)
	{
		//dots_store_NumRemain = (uint8_t)(clusterDet->dots_store_Num - clusterDet->dots_store_NumIn);//当前帧未与存储区目标簇进行聚类的点迹数，理论上=0
		dots_dots_NumRemain = (uint8_t)(dots_dots_Num - dots_dots_NumIn);//当前帧剩余新点迹目标簇个数
		lonelyNumRemain = (uint8_t)(lonelyNum - lonelyNumIn);//当前帧剩余孤点个数

		dotClustId = TWS_DOT_COHESION_NUM + dots_dots_NumIn;//当前帧新的点迹目标簇识别起始ID

		if ((storeEmptyNum > 0) && (dots_dots_NumRemain > 0) || (lonelyNumRemain > 0))
		{
			if (dots_dots_NumRemain >= storeEmptyNum)//存储区仅可新增部分新的点迹目标簇
			{
				clustIdEnd1 = dots_store_NumEnd;//新的点迹目标簇存储起始下标
				dots_dots_NumEnd = storeEmptyNum;//可加入存储区新的目标簇总数=存储区空余目标簇空间个数 
				lonelyNumEnd = 0;
				clusterTargetFun_new_tws(clustIdEnd1, dots_dots_NumEnd, dotClustId, detNum, clustID, measInput, TwsDotCoheStr);//ID判断从(30+ii)开始，函数更新
			}
			else  //存储区可新增新的点迹目标簇+孤点
			{
				//(3-1)remain dots cluster + lonely target
				clustIdEnd1 = dots_store_NumEnd;//新的点迹目标簇存储起始下标
				dots_dots_NumEnd = dots_dots_NumRemain;//可加入存储区新的目标簇总数=当前帧剩余新点迹目标簇个数

				clustIdEnd2 = dots_store_NumEnd + dots_dots_NumRemain;//孤点存储起始下标

				if (dots_dots_NumRemain + lonelyNumRemain <= storeEmptyNum)//存储区可新增（全部新的点迹目标簇+全部孤点）
				{
					lonelyNumEnd = lonelyNumRemain;//可加入存储区的孤点总数=当前帧剩余孤点个数
				}
				else//存储区可新增（全部新的点迹目标簇+部分孤点）
				{
					lonelyNumEnd = storeEmptyNum - dots_dots_NumRemain;//可加入存储区的孤点总数=存储区空余目标簇空间个数-当前帧剩余新点迹目标簇个数
				}

				clusterTargetFun_new_tws(clustIdEnd1, dots_dots_NumEnd, dotClustId, detNum, clustID, measInput, TwsDotCoheStr);//ID判断从(30+ii)开始，函数更新
				lonelyTargetFun_tws(clustIdEnd2, lonelyNumEnd, detNum, clustID, measInput, TwsDotCoheStr);
			}
		}
	}

	//(3)更新目标簇存储区的目标簇总数
	TwsDotCoheStr->dotCoheNum = dots_store_NumEnd + dots_dots_NumEnd + lonelyNumEnd;//the number of target cluster  

	//(4)清空当前帧点迹聚类属性
	memset(clusterDet, 0, sizeof(sClusterDetect));

	//*** 打印输出信息 ***//
	if (TwsDotCoheStr->dotCoheNum > 0)
	{
		for (i_dot = 0; i_dot < TwsDotCoheStr->dotCoheNum; i_dot++)
		{
			for (i_det = 0; i_det < TwsDotCoheStr->TwsDotCohesion[i_dot].dotsNum; i_det++)
			{
#ifdef DEBUG_LOG_HXJ_DEBUG
				my_printf("555555555-TWS-CoheUpdate-555555555 frameID[ %d ], curAziBeamRad[ %d ], dotCoheNum[ %d ], clusterId[ %d ],coheFinish[ %d ],leftAziRad[ %d ], dotsNum[ %d ], dotId[ %d ], frameId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], dotCharac[ %d ]\n", \
					measInput->frameID, measInput->curAziBeamRad, TwsDotCoheStr->dotCoheNum, TwsDotCoheStr->TwsDotCohesion[i_dot].clusterId, TwsDotCoheStr->TwsDotCohesion[i_dot].cohesionFinish, TwsDotCoheStr->TwsDotCohesion[i_dot].leftBoundaryAziRad, TwsDotCoheStr->TwsDotCohesion[i_dot].dotsNum, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].dotId, TwsDotCoheStr->TwsDotCohesion[i_dot].frameId[i_det], TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].range, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].doppler, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].mag, \
					TwsDotCoheStr->TwsDotCohesion[i_dot].dotCluster[i_det].dotCharacteristic);
#endif // DEBUG_LOG_HXJ
			}
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("555555550-TWS-CoheUpdate-555555550 frameID[ %d ], dotCoheNum[ %d ], curAziBeamRad[ %d ]\n", measInput->frameID, TwsDotCoheStr->dotCoheNum, measInput->curAziBeamRad);
#endif // DEBUG_LOG_HXJ
	}

}

//**** TWS模式-(5-1-1)remaining target cluster update to store -- TWS ，新的点迹目标簇更新到存储区
void clusterTargetFun_new_tws(uint8_t clustIdEnd, uint8_t storeNewNum, uint8_t dotClustId, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr)
{
	uint8_t i_det, i_clustObj, i_clustID = 0;
	uint8_t clusterId = 0, isUpdate = 0;
	sDots_Cohesion_vector* dotInput = measInput->dotInput;
	sTwsDotCohesion* TwsDotCohesion = (sTwsDotCohesion*)TwsDotCoheStr->TwsDotCohesion;

	if (storeNewNum > 0)
	{
		memset(&TwsDotCohesion[clustIdEnd], 0, storeNewNum * sizeof(sTwsDotCohesion));//更新的目标簇存储区初始化

		for (i_clustObj = 0; i_clustObj < storeNewNum; i_clustObj++)
		{
			clusterId = dotClustId + i_clustObj;//新加入存储区的(当前帧自行新聚类的点迹簇)，下标标志从(30+i)...
			i_clustID = TwsDotCohesion[clustIdEnd + i_clustObj].dotsNum; //该目标簇已有点迹总数
			isUpdate = 0;

			//****** (1)与目标簇聚类点迹存储 ******//
			for (i_det = 0; i_det < detNum; i_det++)
			{
				if (clustID[i_det] == clusterId)
				{
					isUpdate = 1;
					if (i_clustID < POINTS_IN_CLUSTER_NUM)//i_det点迹可与i_clustObj目标簇聚类，且该目标簇的点迹总数未达上限
					{
						memcpy(&TwsDotCohesion[clustIdEnd + i_clustObj].dotCluster[i_clustID], &dotInput[i_det], sizeof(sDots_Cohesion_vector));/* The dots structer contained in the current target cluster */
						TwsDotCohesion[clustIdEnd + i_clustObj].frameId[i_clustID] = measInput->frameID;
						i_clustID++; //i_clustObj目标簇聚类点迹总数累加
					}
				}
			}

			//****** (2)目标簇状态参数更新 ******//
			if (isUpdate == 1)
			{
				TwsDotCohesion[clustIdEnd + i_clustObj].clusterId = clustIdEnd + i_clustObj;//新加入存储区的目标簇ID,从当前存储区目标簇总数起始
				TwsDotCohesion[clustIdEnd + i_clustObj].dotsNum = i_clustID;//The number of dots in the current target cluster
				TwsDotCohesion[clustIdEnd + i_clustObj].timestamp = measInput->timestamp;//Update the timestamp of the current target cluster
				TwsDotCohesion[clustIdEnd + i_clustObj].leftBoundaryAziRad = measInput->curAziBeamRad;//dotInput[0].aziBeamRad,Update the beam azimuth of the left Boundary
				TwsDotCohesion[clustIdEnd + i_clustObj].cohesionFinish = 0;//新加入存储区的目标簇，凝聚未完成0m
				TwsDotCohesion[clustIdEnd + i_clustObj].unambigDone = 0;
				TwsDotCohesion[clustIdEnd + i_clustObj].unambigVel = 0.f;		// init unambigVel while unambigDone = 0
			}

		}
	}

}


//**** TAS模式-(5.2)存储区目标簇清空+点迹属性清空 ****//
void tarClusterStoreUpdate_tas(sMeasInput* measInput, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion)
{
	uint8_t i_det;

	//(1)清除ccqTAS目标簇空间
	if (TasDotCohesion[0].cohesionFinish == 1)
	{
		memset(&TasDotCohesion[0], 0, sizeof(sTasDotCohesion));
	}

	//(2)清空当前帧点迹聚类属性
	memset(clusterDet, 0, sizeof(sClusterDetect));


	//*** 打印输出信息 ***//
	if (TasDotCohesion[0].dotsNum > 0)
	{
		for (i_det = 0; i_det < TasDotCohesion[0].dotsNum; i_det++)
		{
#ifdef DEBUG_LOG_HXJ_DEBUG
			my_printf("555555555-TAS-CoheStr-555555555 frameID[ %d ], curAziBeamRad[ %d ], coheFinish[ %d ], frameId[ %d ], dotsNum[ %d ], dotId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ], dotCharac[ %d ]\n", \
				measInput->frameID, measInput->curAziBeamRad, TasDotCohesion[0].cohesionFinish, TasDotCohesion[0].frameId[i_det], TasDotCohesion[0].dotsNum, TasDotCohesion[0].dotCluster[i_det].dotId, \
				TasDotCohesion[0].dotCluster[i_det].range, TasDotCohesion[0].dotCluster[i_det].doppler, TasDotCohesion[0].dotCluster[i_det].aziBeamRad * INV_ONE13FORMAT_SCALE, \
				TasDotCohesion[0].dotCluster[i_det].eleBeamRad * INV_ONE13FORMAT_SCALE, TasDotCohesion[0].dotCluster[i_det].mag, TasDotCohesion[0].dotCluster[i_det].dotCharacteristic);
#endif // DEBUG_LOG_HXJ_DEBUG
		}
	}
	else
	{
#ifdef DEBUG_LOG_HXJ_DEBUG
		my_printf("555555550-TAS-CoheUpdate-555555550 frameId[ %d ], dotsNum[ %d ], curAziBeamRad[ %d ], tasTargetId[ %d ]\n", measInput->frameID, TasDotCohesion[0].dotsNum, measInput->curAziBeamRad, measInput->tasTrackStr[0].tasTargetId);
#endif // DEBUG_LOG_HXJ_DEBUG
	}

}



//** 6.report the dots cohesion **//
void dotsCohesion_unitReport(sMeasProcessInst* measInst)
{
	uint8_t i_dot = 0;
	uint8_t dotsNum;
	sMeasOutput* measOutput = (sMeasOutput*)measInst->measOutput;//cohesived targets output to tracking
	sDotsCohesionFrame* dotsCoheOutput = (sDotsCohesionFrame*)measInst->dotsCoheOutput;//cohesived targets report
	sMeasurement_SphPoint* measurement = (sMeasurement_SphPoint*)measOutput->measurement;
	sDotCohPointsIdInfo* pointsIdInfo = (sDotCohPointsIdInfo*)measOutput->pointsIdInfo;
	sCalcAngleInfo* calcAngleInfo = (sCalcAngleInfo*)measOutput->calcAngleInfo;

	dotsNum = measOutput->num;

	memset(dotsCoheOutput, 0, sizeof(sDotsCohesionFrame));

	if (dotsNum > 0)
	{
		dotsCoheOutput->dotCoheObjNum = measOutput->num;
		dotsCoheOutput->tasTargetId = measOutput->tasTargetId;
		dotsCoheOutput->scanType = measOutput->scanType;
		dotsCoheOutput->condenceDone = measOutput->condenceDone;
		//dotsCoheOutput->aziBeamSin = measOutput->aziBeamSin;
		//dotsCoheOutput->eleBeamSin = measOutput->eleBeamSin;

		for (i_dot = 0; i_dot < dotsNum; i_dot++)
		{
			dotsCoheOutput->coheDotsList[i_dot].id = measurement[i_dot].detId;
			dotsCoheOutput->coheDotsList[i_dot].azimuth = (int16_t)(measurement[i_dot].vector.azimuthRad * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].range = (uint32_t)(measurement[i_dot].vector.range * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].elevation = (int16_t)(measurement[i_dot].vector.pitchRad * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].velocity = (int16_t)(measurement[i_dot].vector.doppler * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].x = (int32_t)(measurement[i_dot].x * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].y = (int32_t)(measurement[i_dot].y * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].z = (int32_t)(measurement[i_dot].z * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].mag = (uint16_t)(measurement[i_dot].mag * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].snr = (uint16_t)(measurement[i_dot].snr * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].rcs = (uint16_t)(measurement[i_dot].rcs * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].sinAzim = (int16_t)(measurement[i_dot].sinAzim * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].cosAzim = (int16_t)(measurement[i_dot].cosAzim * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].unambigVel = (int16_t)(measurement[i_dot].unambigVel * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].classification = measurement[i_dot].classification;
			dotsCoheOutput->coheDotsList[i_dot].aziBeamSin = measOutput->aziBeamSin;
			dotsCoheOutput->coheDotsList[i_dot].eleBeamSin = measOutput->eleBeamSin;
			dotsCoheOutput->coheDotsList[i_dot].waveType = measurement[i_dot].waveType;
			dotsCoheOutput->coheDotsList[i_dot].flag_moving = measurement[i_dot].flag_moving;
			dotsCoheOutput->coheDotsList[i_dot].unambigVelDone = measurement[i_dot].unambigVelDone;

			dotsCoheOutput->coheDotsList[i_dot].detectionNum = pointsIdInfo[i_dot].detectionNum;
			memcpy(dotsCoheOutput->coheDotsList[i_dot].detectionId, pointsIdInfo[i_dot].detectionId, POINTS_IN_CLUSTER_NUM * sizeof(uint16_t));
			memcpy(dotsCoheOutput->coheDotsList[i_dot].detectionFrameId, pointsIdInfo[i_dot].frameId, POINTS_IN_CLUSTER_NUM * sizeof(uint32_t));

			dotsCoheOutput->coheDotsList[i_dot].MagBeamList[0] = (int16_t)(10.f * log10f(calcAngleInfo[i_dot].MagBeamList[0]) * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].MagBeamList[1] = (int16_t)(10.f * log10f(calcAngleInfo[i_dot].MagBeamList[1]) * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].MagBeamList[2] = (int16_t)(10.f * log10f(calcAngleInfo[i_dot].MagBeamList[2]) * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].MagBeamList[3] = (int16_t)(10.f * log10f(calcAngleInfo[i_dot].MagBeamList[3]) * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].aziBeamList[0] = (int16_t)(calcAngleInfo[i_dot].azimuthRadList[0] * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].aziBeamList[1] = (int16_t)(calcAngleInfo[i_dot].azimuthRadList[1] * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].eleBeamList[0] = (int16_t)(calcAngleInfo[i_dot].eleBeamRadList[0] * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].eleBeamList[1] = (int16_t)(calcAngleInfo[i_dot].eleBeamRadList[1] * RAD2DEG * ONE6FORMAT_SCALE);
			dotsCoheOutput->coheDotsList[i_dot].ifCalcAzi = calcAngleInfo[i_dot].ifCalcAzi;
			dotsCoheOutput->coheDotsList[i_dot].ifCalcEle = calcAngleInfo[i_dot].ifCalcEle;
		}
	}

}

//***************************************  Subfunction area  **************************************//





//=============================== old =================================//

////** TAS mode **//
//void tas_measurement_process(sAlgObjData* algObj)
//{
//    uint8_t i_tas, add, iTarget;
//    uint8_t numTas; //number of clustering targets in cache TWS
//    uint16_t tasObjId;			// index of tas object, only valid for tas scan mode
//    uint8_t tasBeamTotal;
//    uint8_t cacheBeamTarNum[COCH_BEAM_NUM_TAS] = { 0 };
//    sDetectFrame* detClutProOut = (sDetectFrame*)gDetObjOut;//杂波处理输出检测点结构体
//    sMultiBeamMeas* cacheBeam = (sMultiBeamMeas*)gMultiBeamMeas;//输出目标缓存结构体
//    sDetectFrame* detInst = (sDetectFrame*)algObj->detectsListOutput;//输入的波位检测信息
//    sDispatchInst* dispatchInst = (sDispatchInst*)algObj->dispatch_inst;//输入的波位调度信息
//    sDispatchOutput* dispatchOutput = dispatchInst->dispatchOutput;
//    sMeasProcessInst* measInst = (sMeasProcessInst*)algObj->measurementProcess_inst;//波位点迹信息存储区   
//    sTrackingInst* tracking_inst = (sTrackingInst*)(algObj->tracking_inst);//跟踪信息
//    sTrackingTarget* tasTrackStr;
//    sDetTrackFrame* TrackDetStr = gTrackDetStr;//TAS目标DBScan聚类筛选的检测点结构体
//    sDetTrackFrame* cacheTas;
//    sMeasOutput* measObjOut;
//
//
//    measObjOut = measInst->measOutput;//点迹凝聚输出结构体
//
//    memset(TrackDetStr, 0, sizeof(sDetTrackFrame)); /* 跟踪目标聚类检测点结构体初始化 */
//    memset(measObjOut, 0, sizeof(sMeasOutput));     /* 输出结构体初始化 */
//
//    //*** 状态参数赋值 ***//
//    measObjOut->scanType = TAS_SCAN;// dispatchOutput->trackTwsTasFlag;//TWS TAS模式
//    measObjOut->tasTargetId = dispatchOutput->tasObjId;
//    measObjOut->timestamp = (float)(detInst->timestamp) / 1000.f;//ms to s
//    measObjOut->condenceDone = 0;//初始，凝聚未完成
//	measObjOut->waveType = detInst->waveType;
//
//    for (i_tas = 0; i_tas < detInst->detectObjNum; i_tas++)
//    {
//#ifdef DEBUG_LOG_HXJ
//        my_printf("111111111-TAS-111111111 detNum[ %d ], detId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ]\n", \
//            detInst->detectObjNum, detInst->detPointsList[i_tas].id,
//            detInst->detPointsList[i_tas].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_tas].velocity * INV_ONE6FORMAT_SCALE, \
//            detInst->detPointsList[i_tas].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_tas].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
//            detInst->detPointsList[i_tas].mag * INV_ONE6FORMAT_SCALE);
//#endif // DEBUG_LOG_HXJ
//    }
//
//     //*** 0、杂波处理 ***//
//    clutterProcessing(detInst, detClutProOut);
//
//    //*** 1.读取TAS跟踪目标信息 ***//
//    tasObjId = dispatchOutput->tasObjId;
//    for (iTarget = 0; iTarget < tracking_inst->trajInfoOutput->trackObjNum; iTarget++)
//    {
//        if (tracking_inst->trajInfoOutput->trajList[iTarget].id == tasObjId)
//        {
//            tasTrackStr = &tracking_inst->trajInfoOutput->trajList[iTarget];
//            break;
//        }
//    }
//    //*** 2、波位内点迹初筛+存储 ***//
//	tasBeamTotal = dispatchOutput->tasBeamTotal;//调度扫描的TAS波位总数
//    detChoseWithinBeam_tas(detClutProOut, tasTrackStr, cacheBeam, tasBeamTotal);//clusterObjIn
//
//    //*** 3.集齐TAS所有波位后进行波位间凝聚处理 ***//
//    
//    numTas = cacheBeam->numTas;
//    if (numTas != tasBeamTotal) //if (numTas != COCH_BEAM_NUM_TAS) 
//    {
//        return;
//    }
//    else
//    {      
//        measObjOut->condenceDone = 1;//TAS所有波位搜索已完成
//		cacheBeam->numTas = 0;
//        cacheTas = (sDetTrackFrame*)cacheBeam->multiBeam_tas;
//        add = 0;
//        for (i_tas = 0; i_tas < COCH_BEAM_NUM_TAS; i_tas++)//the last one is the courrent beam
//        {
//            cacheBeamTarNum[i_tas] = cacheTas[i_tas].detectFrame.detectObjNum;
//            memcpy(&TrackDetStr->detectFrame.detPointsList[add], cacheTas[i_tas].detectFrame.detPointsList, cacheBeamTarNum[i_tas] * sizeof(sDetectPoint));
//            TrackDetStr->aziBeamSin[i_tas] = cacheTas[i_tas].aziBeamSin[0];
//            TrackDetStr->eleBeamSin[i_tas] = cacheTas[i_tas].eleBeamSin[0];
//                      
//            add = add + cacheBeamTarNum[i_tas];
//        }
//        TrackDetStr->detectFrame.detectObjNum = add;//TAS目标4波位可聚类检测点总数
//        TrackDetStr->detectFrame.frameID = detClutProOut->frameID;
//        TrackDetStr->detectFrame.timestamp = detClutProOut->timestamp;
//        TrackDetStr->detectFrame.trackTwsTasFlag = detClutProOut->trackTwsTasFlag;
//        TrackDetStr->detectFrame.detectObjByte = detClutProOut->detectObjByte;
//
//        //TAS所有波位可聚类目标进行聚类处理
//        if (TrackDetStr->detectFrame.detectObjNum < 1)
//        {
//            return;
//        }
//        else
//        { 
//             detClusteringBetweenBeam_tas(TrackDetStr, tasTrackStr, cacheBeamTarNum, tasBeamTotal, measObjOut);
//        }
//    }
//
//    for (i_tas = 0; i_tas < measObjOut->num; i_tas++)
//    {
//#ifdef DEBUG_LOG_HXJ
//        my_printf("22222222-TAS-2222222222 measNum[ %d ], measId[ %d ],condenceDone[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ]\n", \
//            measObjOut->num, measObjOut->measurement[i_tas].detId, measObjOut->condenceDone,
//            measObjOut->measurement[i_tas].vector.range, measObjOut->measurement[i_tas].vector.doppler, \
//            measObjOut->measurement[i_tas].vector.azimuthRad, measObjOut->measurement[i_tas].vector.pitchRad, \
//            measObjOut->measurement[i_tas].mag);
//#endif // DEBUG_LOG_HXJ
//    }
//
//}


////** TWS mode **//
//void tws_measurement_process(sAlgObjData *algObj)
//{
//    uint8_t i_tws, aziGap = 0, eleGap = 0;
//    uint8_t clusterTarNum, cacheTarNum;
//    uint8_t aziComplete = 0, eleComplete = 0;
//    uint8_t numTws; //number of clustering targets in cache TWS
//    uint8_t calcAngleMode = 0;//测角模式：1方位，2俯仰
//    int16_t aziBeamSin_meas = 0, eleBeamSin_meas = 0;
//    sDetectFrame* detClutProOut = (sDetectFrame*)gDetObjOut;//杂波处理输出检测点结构体
//    sMeasOutput* clusterObjIn = (sMeasOutput*)gClusterObj;//波位内凝聚输出目标结构体
//    sMultiBeamMeas* cacheBeam = (sMultiBeamMeas*)gMultiBeamMeas;//输出目标缓存结构体
//    sMeasOutput* measObjOut;
//    sMeasOutput* cacheMeasBeam;
//    sMeasurement_SphPoint* cacheMeasStr;
//    sDetectFrame* detInst = (sDetectFrame*)algObj->detectsListOutput;//输入的波位检测信息
//    sDispatchInst* dispatchInst = (sDispatchInst*)algObj->dispatch_inst;//输入的波位检测信息
//    //sDispatchOutput *dispatchOutput = dispatchInst->dispatchOutput;
//    sMeasProcessInst* measInst = (sMeasProcessInst*)algObj->measurementProcess_inst;//波位点迹信息存储区   
//    measObjOut = measInst->measOutput;//点迹凝聚输出结构体
//
//    memset(measObjOut, 0, sizeof(sMeasOutput));/* 输出结构体初始化 */
//
//     for (i_tws = 0; i_tws < detInst->detectObjNum; i_tws++)
//    {
//#ifdef DEBUG_LOG_HXJ
//        my_printf("111111111-TWS-111111111 frameId %d, detNum[ %d ], detId[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],rSNR %d, dSNR %d\n", \
//            detInst->frameID, detInst->detectObjNum, detInst->detPointsList[i_tws].id,
//            detInst->detPointsList[i_tws].range * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_tws].velocity * INV_ONE6FORMAT_SCALE, \
//            detInst->detPointsList[i_tws].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD, detInst->detPointsList[i_tws].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD, \
//            detInst->detPointsList[i_tws].mag * INV_ONE6FORMAT_SCALE, detInst->detPointsList[i_tws].detProperty.classification, detInst->detPointsList[i_tws].detProperty.cohesionOkFlag);
//#endif // DEBUG_LOG_HXJ
//    }
//
//    //*** 0、杂波处理 ***//
//    clutterProcessing(detInst, detClutProOut);
//
//    //*** 1、波位内点迹处理 ***//
//    //clusteringWithinBeam_tws(detClutProOut, cacheBeam, clusterObjIn);//detInst
//
//    memcpy(measObjOut, clusterObjIn, sizeof(sMeasOutput));/* 输出结构体赋值当前波位信息 */
//
//    //*** 2、遍历存储区，更新输出结构体 ***//
//    clusterTarNum = clusterObjIn->num;
//    numTws = cacheBeam->numTws;
//    if (clusterTarNum > 0)
//    {          
//        if (numTws > 1)
//        {   
//            for ( i_tws = 0; i_tws < numTws-1; i_tws++)//the last one is the courrent beam
//            {              
//                cacheMeasBeam = &cacheBeam->multiBeam_tws[i_tws];
//                cacheTarNum = cacheMeasBeam->num;
//
//                if (cacheTarNum > 0)
//                {
//                    cacheMeasStr = &cacheMeasBeam->measurement[0];
//                    aziBeamSin_meas = clusterObjIn->measurement[0].aziBeamSin;
//                    eleBeamSin_meas = clusterObjIn->measurement[0].eleBeamSin;
//                   
//                     if ((aziBeamSin_meas != cacheMeasStr->aziBeamSin) && (eleBeamSin_meas == cacheMeasStr->eleBeamSin))
//                    {
//                        calcAngleMode = 1;//测方位角
//                        //clusteringBetweenBeam_tws(calcAngleMode, cacheMeasBeam, clusterObjIn, measObjOut);
//                        clusteringBetweenBeam_tws(calcAngleMode, cacheMeasBeam, measObjOut);
//                        aziComplete = 1;
//                        aziGap = (numTws - 1 - i_tws);
//#ifdef DEBUG_LOG_HXJ
//                       /* my_printf("111111111-Azi-111111111 frame_gap[ %d ],calcAngleMode[ %d ]\n", \
//                            (numTws-1-i_tws), calcAngleMode);*/
//#endif // DEBUG_LOG_HXJ
//                    }
//                    else if ((aziBeamSin_meas == cacheMeasStr->aziBeamSin) && (eleBeamSin_meas != cacheMeasStr->eleBeamSin))
//                    {
//                         if (i_tws == numTws - 2)
//                         {
//                             calcAngleMode = 2;//测俯仰角
//                             //clusteringBetweenBeam_tws(calcAngleMode, cacheMeasBeam, clusterObjIn, measObjOut);
//                             clusteringBetweenBeam_tws(calcAngleMode, cacheMeasBeam, measObjOut);
//                             eleComplete = 1;
//                             eleGap = (numTws - 1 - i_tws);
//#ifdef DEBUG_LOG_HXJ
//                            /* my_printf("111111111-Ele-111111111 frame_gap[ %d ],calcAngleMode[ %d ]\n", \
//                             (numTws - 1 - i_tws), calcAngleMode);*/
//#endif // DEBUG_LOG_HXJ
//                         }
//                    }
//
//                }
//            }
//        }
//    }
//
//  
//    //*** 状态参数赋值 ***//
//    measObjOut->scanType = TWS_SCAN;//TWS TAS模式
//    measObjOut->tasTargetId = -1;
//    measObjOut->timestamp = (float)(detInst->timestamp) / 1000.f;//ms to s
//    measObjOut->condenceDone = 0;//初始，凝聚未完成
//	measObjOut->waveType = detInst->waveType;
//    //凝聚完成标志
//    if (aziComplete + eleComplete == 2) 
//    {
//        measObjOut->condenceDone = 1;
//    }
//#ifdef DEBUG_LOG_ZQ
//    //my_printf("frameId %d measOut num %d conde %d  r a p v [%.2f %.2f %.2f %.2f]", \
//        detInst->frameID, measObjOut->num, \
//        measObjOut->condenceDone, measObjOut->measurement[0].vector.range, \
//        measObjOut->measurement[0].vector.azimuthRad*RAD2DEG, \
//        measObjOut->measurement[0].vector.pitchRad*RAD2DEG, \
//        measObjOut->measurement[0].vector.doppler);
//#endif // DEBUG_LOG_HXJ
//
//    for (i_tws = 0; i_tws < measObjOut->num; i_tws++)
//    {
//#ifdef DEBUG_LOG_HXJ
//        my_printf("22222222-TWS-2222222222 frameId %d, measNum[ %d ],measId[ %d ],numTws[ %d ],aziComp[ %d ],eleComp[ %d ],condenceDone[ %d ], r v a p m[ %.5f,%.5f,%.5f,%.5f,%.5f ],detNum[ %d ],detID[ %d,%d,%d,%d,%d,%d,%d,%d ]\n", \
//            detInst->frameID, measObjOut->num, measObjOut->measurement[i_tws].detId, numTws, aziGap, eleGap, measObjOut->condenceDone,
//            measObjOut->measurement[i_tws].vector.range, measObjOut->measurement[i_tws].vector.doppler, \
//            measObjOut->measurement[i_tws].vector.azimuthRad, measObjOut->measurement[i_tws].vector.pitchRad, \
//            measObjOut->measurement[i_tws].mag, measObjOut->measurement[i_tws].detectionNum, measObjOut->measurement[i_tws].detectionId[0], measObjOut->measurement[i_tws].detectionId[1], measObjOut->measurement[i_tws].detectionId[2], \
//            measObjOut->measurement[i_tws].detectionId[3], measObjOut->measurement[i_tws].detectionId[4], measObjOut->measurement[i_tws].detectionId[5], measObjOut->measurement[i_tws].detectionId[6], measObjOut->measurement[i_tws].detectionId[7]);
//#endif // DEBUG_LOG_HXJ
//    }
//
//}



//**** 杂波处理模块 ****//
//void clutterProcessing(sDetectFrame* detInst, sDetectFrame* detClutProOut)
//{
//    uint8_t n_det, n_out;
//    sDetectPoint* detPointStr = detInst->detPointsList;
//    uint8_t numPointIn;
//    uint8_t isMirrorFreqPoint[MAX_NUM_DETECTS] = { 0 };
//    float range,mag;
//    float mag_thershold;
//    float doppler = 0.f;
//    float azimuth = 0.f;    
//
//    numPointIn = detInst->detectObjNum;
//
//    memset(detClutProOut, 0, sizeof(sDetectFrame));
//
//    // （1）镜频抑制模块：速度维上中心对称且幅度相差较大认为是镜频（IQ通道幅度不平衡，相位不正交等原因导致）
//    ifMirrorFreqPoint(detPointStr, numPointIn, isMirrorFreqPoint);
//    
//    n_out = 0;
//    for (n_det = 0; n_det < numPointIn; n_det++)
//    {
//        range = detPointStr[n_det].range * INV_ONE6FORMAT_SCALE;
//        mag = detPointStr[n_det].mag * INV_ONE6FORMAT_SCALE;
//        doppler = detPointStr[n_det].velocity* INV_ONE6FORMAT_SCALE;
//        azimuth = detPointStr[n_det].azimuth*INV_ONE6FORMAT_SCALE;
//        mag_thershold = ((range <= rangeThershold[0]) ? magThershold[0] : ((range <= rangeThershold[1]) ? magThershold[1] : ((range <= rangeThershold[2]) ? magThershold[2] : ((range <= rangeThershold[3]) ? magThershold[3] : ((range <= rangeThershold[4]) ? magThershold[4] : ((range <= rangeThershold[5]) ? magThershold[5] : ((range <= rangeThershold[6]) ? magThershold[6] : ((range <= rangeThershold[7]) ? magThershold[7] : ((range <= rangeThershold[8]) ? magThershold[8] : ((range <= rangeThershold[9]) ? magThershold[9] : ((range <= rangeThershold[10]) ? magThershold[10] : magThershold[11])))))))))));
//#ifdef DEMO_ACUR
//        // TODO: supress clutter
//        /*if (fabsf(doppler) > 5.3f || ((range > 228) && (range < 241) && fabsf(doppler) > 3.5f&&fabsf(doppler) < 5.5f))
//            continue;
//        if (range > 292.f && range < 298.f)
//            continue;*/
//        // TODO: supress clutter
////		if (fabsf(azimuth) > 7.f)
//        //if (fabsf(azimuth) > 15.f)//azi ±20, ele ±5
//        //	continue;
//		if (isReflectPoint(range, doppler, mag, n_det, detPointStr, numPointIn))
//		{
//#ifdef DEBUG_LOG_ZQ
//			my_printf("reflect point %d", detPointStr[n_det].id);
//#endif // DEBUG_ZQ
//			continue;
//		}
//           
//        if (isPowerSupplyPoint(range, doppler, mag, n_det, detPointStr, numPointIn))
//        {
//#ifdef DEBUG_LOG_ZQ
//            my_printf("power point %d", detPointStr[n_det].id);
//#endif // DEBUG_ZQ
//            continue;
//        }
//#endif // DEMO_ACUR
//
//        if (isMirrorFreqPoint[n_det] == 1)
//        { 
//#ifdef DEBUG_LOG_ZQ
//			my_printf("mirror freq point %d", detPointStr[n_det].id);
//#endif // DEBUG_ZQ
//            continue;
//
//        }           
//        
//        if (mag >= mag_thershold)
//        {
//            memcpy(&detClutProOut->detPointsList[n_out], &detPointStr[n_det], sizeof(sDetectPoint));
//            n_out++;
//        }
//		else
//		{
//#ifdef DEBUG_LOG_ZQ
//			my_printf("detect mag is too small %d", detPointStr[n_det].id);
//#endif // DEBUG_ZQ
//
//		}
//    }
//    detClutProOut->detectObjNum = n_out;
//    detClutProOut->detectObjByte = detInst->detectObjByte;
//    detClutProOut->frameID = detInst->frameID;
//    detClutProOut->timestamp = detInst->timestamp;
//    detClutProOut->trackTwsTasFlag = detInst->trackTwsTasFlag;
//    detClutProOut->waveType = detInst->waveType;
//}
//
//uint8_t isReflectPoint(float range, float doppler, float mag,uint8_t n_det, sDetectPoint* detPointStr,uint8_t nNum)
//{
//	uint8_t iDet = 0;
//	float range0, mag_i;
//	float doppler0 = 0.f;
//	uint8_t reflectFlag = 0;
//	uint8_t multiRefNum = 11;
//	uint8_t iReflect = 2;
//	float dopplerAmbi = 0.f;
//	float dopplerScope = 12.8823f;
//	int n = 0;
//	for (iDet = 0; iDet < nNum; iDet++)
//	{
//		range0 = detPointStr[iDet].range * INV_ONE6FORMAT_SCALE;
//		mag_i = detPointStr[iDet].mag * INV_ONE6FORMAT_SCALE;
//		doppler0 = detPointStr[iDet].velocity* INV_ONE6FORMAT_SCALE;
//		if (iDet == n_det)
//			continue;
//		if (range0 > range)
//			continue;
//		for (iReflect = 2; iReflect < multiRefNum; iReflect++)
//		{
//			n = (int)floor(((float)(iReflect)*doppler0 + 6.649f) / dopplerScope);
//			if (n == 0)
//			{
//				dopplerAmbi = (float)(iReflect)*doppler0;
//			}
//			else
//			{
//				dopplerAmbi = (float)(iReflect)*doppler0 - (float)(n)*dopplerScope;
//			}
//			if (fabsf((float)iReflect*range0 - range) < 5.f+(iReflect-2.f)*2.f&& \
//				fabsf(dopplerAmbi - doppler) < 0.5f + (iReflect - 2.f)*0.1f&& \
//				mag_i > mag)
//			{
//				reflectFlag = 1;
//				break;
//			}
//		}
//		if (reflectFlag == 1)
//			break;
//	}
//	return reflectFlag;
//}
//
//uint8_t isPowerSupplyPoint(float range, float doppler, float mag, uint8_t n_det, sDetectPoint* detPointStr, uint8_t nNum)
//{
//	uint8_t iDet = 0;
//	float range0;
//	float doppler0 = 0.f;
//	float range304 = 0.f;
//	float doppler304 = 0.f;
//	float range304_ = 0.f;
//	float doppler304_ = 0.f;
//	uint8_t powerSupplyFlag = 0;
//	float dopplerAmbi = 0.f;
//	uint8_t powerSupply304Flag = 0;
//	float dopplerScope = 12.8823f;
//	int n = 0;
//	if (range > 300.f && range < 308.f)
//	{
//		powerSupplyFlag = 1;
//		return powerSupplyFlag;
//	}
//	for (iDet = 0; iDet < nNum; iDet++)
//	{
//		range304_ = detPointStr[iDet].range * INV_ONE6FORMAT_SCALE;
//		if (range304_ > 300.f&&range304_ < 305.f)
//		{
//			doppler304_ = detPointStr[iDet].velocity* INV_ONE6FORMAT_SCALE;
//			if (fabsf(doppler304_) > fabsf(doppler304))
//			{
//				doppler304 = doppler304_;
//				range304 = range304_;
//				powerSupply304Flag = 1;
//			}
//		}
//	}
//	if (powerSupply304Flag == 0)
//	{
//		return powerSupplyFlag;
//	}
//	if (range > 150.f && powerSupply304Flag == 1)
//	{
//		for (iDet = 0; iDet < nNum; iDet++)
//		{
//			range0 = detPointStr[iDet].range * INV_ONE6FORMAT_SCALE;
//			doppler0 = detPointStr[iDet].velocity* INV_ONE6FORMAT_SCALE;
//			if (iDet == n_det)
//				continue;
//			if (range0 > range || range0 > 150.f)
//				continue;
//			n = (int)floor((doppler + doppler0 + 6.649f) / dopplerScope);
//			if (n == 0)
//			{
//				dopplerAmbi = doppler + doppler0;
//			}
//			else
//			{
//				dopplerAmbi = doppler + doppler0 - (float)(n)*dopplerScope;
//			}
//
//			if ((fabsf(range + range0 - 304.f) < 5.f&&fabsf(dopplerAmbi - doppler304) < 1.f))
//			{
//				powerSupplyFlag = 1;
//				break;
//			}
//			n = (int)floor((doppler - doppler0 + 6.649f) / dopplerScope);
//			if (n == 0)
//			{
//				dopplerAmbi = doppler - doppler0;
//			}
//			else
//			{
//				dopplerAmbi = doppler - doppler0 - (float)(n)*dopplerScope;
//			}
//			if ((fabsf(range - range0 - 304.f) < 5.f&&fabsf(dopplerAmbi - doppler304) < 0.5f))
//			{
//				powerSupplyFlag = 1;
//				break;
//			}
//		}
//	}
//	return powerSupplyFlag;
//}

//void ifMirrorFreqPoint(uint8_t detNum, sDots_Cohesion_vector* dotInput)
//{
//    uint8_t iDet = 0, jDet = 0;
//    int16_t dopplerChn_i, dopplerChn_j;
//    float mag_i, mag_j;   
//    
//    for (iDet = 0; iDet < detNum; iDet++)
//    {
//        mag_i = dotInput[iDet].mag;
//        dopplerChn_i = dotInput[iDet].dopplerChn;
//
//        if (dotInput[iDet].dotCharacteristic == MIRROR_FREQUENCE_POINT)
//        {
//            continue;
//        }           
//        for (jDet = iDet + 1; jDet < detNum; jDet++)
//        {
//            mag_j = dotInput[jDet].mag;
//            dopplerChn_j = dotInput[jDet].dopplerChn;
//            if (dotInput[jDet].dotCharacteristic == MIRROR_FREQUENCE_POINT)
//            {
//                continue;
//            }
//            if (fabsf(dopplerChn_i + dopplerChn_j - NUM_DOPPLER_BIN) < MIR_FREQ_BIN_DIFF)
//            {
//                if (mag_i > (MIR_FREQ_MAG_RATIO * mag_j))
//                {
//                    dotInput[jDet].dotCharacteristic = MIRROR_FREQUENCE_POINT;
//                }
//                else if (mag_j > (MIR_FREQ_MAG_RATIO * mag_i))
//                {
//                    dotInput[iDet].dotCharacteristic = MIRROR_FREQUENCE_POINT;
//                    break;
//                }                
//            }
//        }
//    }
//
//}


////**** TWS模式-波位内点迹处理模块 ****//
//void clusteringWithinBeam_tws(sDetectFrame * detClutProOut, sMultiBeamMeas* cacheBeam, sMeasOutput* clusterObjIn)
//    {
//        uint8_t n_det, i_current, i_neigh, i_beam;
//        uint8_t* neighLast;
//        uint8_t* neighCurrent;
//        uint8_t neighCount, newCount;
//        uint8_t clustTarID = 0, lonelyTarID = 0;
//        uint8_t numPoints = 0;
//        uint8_t visited_in[MAX_NUM_DETECTS] = { 0 };
//        uint8_t scope[MAX_NUM_DETECTS] = { 0 };
//        uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };
//        uint8_t numTWS;//, numTAS
//        sDetectPoint* detPointStr = detClutProOut->detPointsList;
//        sClusterDetect* clusterDet = gClusterDet;//波位内聚类点迹缓存结构体
//        sMeasOutput* cacheTws;
//
//
//        //**** 1、目标属性归类 ****//
//        numPoints = detClutProOut->detectObjNum;
//        memset(clusterDet->clusterID, 0, MAX_NUM_DETECTS * sizeof(uint8_t));
//        memset(visited_in, CLUSTER_POINT_UNKNOWN, numPoints * sizeof(uint8_t));
//
//        // Init the clusterID of points are CLUSTER_POINT_LONELY
//        for (n_det = 0; n_det < numPoints; n_det++)
//        {
//            clusterDet->clusterID[n_det] = CLUSTER_POINT_LONELY;
//        }
//        // scan through all the points to find its neighbors
//        for (n_det = 0; n_det < numPoints; n_det++)
//        {
//            if (visited_in[n_det] != CLUSTER_POINT_VISITED)
//            {
//                visited_in[n_det] = CLUSTER_POINT_VISITED;
//
//                neighCurrent = neighLast = neighbors;
//                // scope is the local copy of visit
//                memcpy(scope, visited_in, numPoints * sizeof(uint8_t));
//
//                neighCount = dot_dot_DBscan_findNeighbors(
//                    detPointStr, n_det, neighLast, numPoints,
//                    scope, &newCount);
//
//#ifdef DEBUG_LOG_HXJ
//            /*    my_printf("det[%d]: neightCount: %d\n", \
//                    n_det, neighCount);*/
//#endif
//            /* The cluster consists of the point itself and its neighbours. */
//                if (neighCount < MIN_NUM_POINTS_IN_CLUSTER - 1)
//                {
//                    // This point is lonely point
//                    clusterDet->clusterID[n_det] = CLUSTER_POINT_LONELY;
//                }
//                else
//                {
//                    // This point belongs to a New Cluster
//                    clustTarID++;                                // New cluster ID
//                    clusterDet->clusterID[n_det] = clustTarID;      // This point belong to this cluster
//
//                    // tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
//                    for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//                    {
//                        i_current = neighLast[i_neigh];//与当前n_det聚类的ID
//                        scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
//                    }
//                    neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位
//
//                    while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
//                    {
//                        // Explore the neighborhood
//                        i_current = *neighCurrent++;               // Take point from the neighborhood
//                        clusterDet->clusterID[i_current] = clustTarID; // All points from the neighborhood also belong to this cluster
//                        visited_in[i_current] = CLUSTER_POINT_VISITED;
//
//                        neighCount = dot_dot_DBscan_findNeighbors(
//                            detPointStr, i_current, neighLast, numPoints,
//                            scope, &newCount);
//
//                        if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
//                        {
//                            for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//                            {
//                                i_current = neighLast[i_neigh];
//                                scope[i_current] = CLUSTER_POINT_VISITED;
//                            }
//                            neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
//                        }
//                    }
//                    if (clustTarID >= MAX_NUM_DETECTS)
//                    {
//                        return;
//                    }
//                }
//            }
//        }
//
//        clusterDet->clusterNum = clustTarID;//可聚类目标数
//
//        //孤点查找
//        for ( n_det = 0; n_det < numPoints; n_det++)
//        {
//            if (clusterDet->clusterID[n_det] == CLUSTER_POINT_LONELY)
//            {
//                lonelyTarID = lonelyTarID + 1;               
//            }
//        }
//
//        clusterDet->lonelyNum = lonelyTarID;//孤点目标
//
//#ifdef DEBUG_LOG_HXJ
//        //my_printf("numPoints clustTarID lonelyTarID [%d %d %d]\n", numPoints, clustTarID, lonelyTarID);
//#endif // DEBUG_LOG_HXJ
//
//        //**** 2、归类目标进行聚类 + ****//
//        clusteringFunInBeam_tws(clusterDet, detClutProOut, clusterObjIn);
//
//        //**** 3、波位信息存储 ****//
//        if (detClutProOut->trackTwsTasFlag == TWS_SCAN)
//        {
//            numTWS = cacheBeam->numTws;
//            cacheTws = (sMeasOutput*)cacheBeam->multiBeam_tws;
//            if (numTWS < COCH_BEAM_NUM_TWS)
//            {
//                memcpy(&cacheTws[cacheBeam->numTws], clusterObjIn, sizeof(sMeasOutput));
//                cacheBeam->numTws++;
//            }
//            else
//            {
//                //集齐8个波位后，前4个波位直接清零，后4个波位顺移到前4个波位，从第5个波位开始赋值
//                for (i_beam = COCH_BEAM_NUM_TWS/2; i_beam < COCH_BEAM_NUM_TWS; i_beam++)
//                {
//                    memcpy(&cacheTws[i_beam - COCH_BEAM_NUM_TWS/2], &cacheTws[i_beam], sizeof(sMeasOutput));
//                    memset(&cacheTws[i_beam], 0, sizeof(sMeasOutput));
//                }
//                cacheBeam->numTws = (COCH_BEAM_NUM_TWS/2);
//                memcpy(&cacheTws[cacheBeam->numTws], clusterObjIn, sizeof(sMeasOutput));
//                cacheBeam->numTws++;
//            }
//        }      
//
//    }


////**** TWS模式-波位间点迹处理模块 ****//
//void clusteringBetweenBeam_tws(uint8_t calcAngleMode, sMeasOutput* cacheMeasBeam, sMeasOutput* clusterObjIn, sMeasOutput* measObjOut)
//{
//    uint8_t i_curObj, i_cacheObj;
//    uint8_t N_curObj = clusterObjIn->num;//当前波位内点迹总数
//    uint8_t N_cacheObj = cacheMeasBeam->num; //缓存区波位内聚类后的目标总数
//    sClusterMeasure* clusterMeas = gClusterMeas;//波位间聚类点迹缓存结构体
//        
//        
//    //**** 1、当前波位及存储区波位重组：当前波位+缓存波位 ****//
//    memset(clusterMeas, 0, sizeof(sClusterMeasure));
//    for (i_curObj = 0; i_curObj < N_curObj; i_curObj++)
//    {
//        memcpy(&clusterMeas->measurement[i_curObj], &clusterObjIn->measurement[i_curObj], sizeof(sMeasurement_SphPoint));
//    }
//    for (i_cacheObj = 0; i_cacheObj < N_cacheObj; i_cacheObj++)
//    {
//        memcpy(&clusterMeas->measurement[N_curObj+i_cacheObj], &cacheMeasBeam->measurement[i_cacheObj], sizeof(sMeasurement_SphPoint));
//    }
//
//    clusterMeas->clusterMeasNum = N_curObj + N_cacheObj;
//
//
//    //**** 2、DNScan遍历重组波位进行聚类分类 ****//
//    cluster_DBscan(clusterMeas, N_curObj);//N_curObj为孤点遍历范围
//
//
//    //**** 3、归类目标进行聚类 + 比幅测角 ****//
//    clusteringFunBetweenBeams(N_curObj, N_cacheObj, calcAngleMode, clusterMeas, measObjOut);
//
//    /*measObjOut->scanType = clusterObjIn->scanType;
//    measObjOut->tasTargetId = clusterObjIn->tasTargetId;
//    measObjOut->timestamp = clusterObjIn->timestamp;*/
//
//
//}

////**** TWS模式-波位间点迹处理模块 ****//
//    void clusteringBetweenBeam_tws(uint8_t calcAngleMode, sMeasOutput* cacheMeasBeam, sMeasOutput* measObjOut)
//    {
//        uint8_t i_curObj, i_cacheObj;
//        uint8_t N_curObj = measObjOut->num;//当前波位处理后的点迹总数
//        uint8_t N_cacheObj = cacheMeasBeam->num; //缓存区波位内聚类后的目标总数
//        sClusterMeasure* clusterMeas = gClusterMeas;//波位间聚类点迹缓存结构体
//
//        //**** 1、当前波位及存储区波位重组：当前波位(measObjOut)+缓存波位(cacheMeasBeam) ****//
//        memset(clusterMeas, 0, sizeof(sClusterMeasure));
//        for (i_curObj = 0; i_curObj < N_curObj; i_curObj++)
//        {
//            memcpy(&clusterMeas->measurement[i_curObj], &measObjOut->measurement[i_curObj], sizeof(sMeasurement_SphPoint));
//        }
//        for (i_cacheObj = 0; i_cacheObj < N_cacheObj; i_cacheObj++)
//        {
//            memcpy(&clusterMeas->measurement[N_curObj + i_cacheObj], &cacheMeasBeam->measurement[i_cacheObj], sizeof(sMeasurement_SphPoint));
//        }
//
//        clusterMeas->clusterMeasNum = N_curObj + N_cacheObj;
//
//        //**** 2、DNScan遍历重组波位进行聚类分类 ****//
//        cluster_DBscan(clusterMeas, N_curObj);//N_curObj为孤点遍历范围
//
//        //**** 3、归类目标进行聚类 + 比幅测角,measObjOut将重新赋值 ****//
//        clusteringFunBetweenBeams(N_curObj, N_cacheObj, calcAngleMode, clusterMeas, measObjOut);
//
//    }



////**** TAS模式-波位内点迹处理模块 ****//
///**
//	* @b Description
//	* @n
//	*      .
//	* 
//	* @param[in] detClutProOut
//	* @param[in] tasTrackStr
//	* @param[in] cacheBeam
//	* @param[in] tasBeamTotal
//	*       
//	*       
//	*/
//void detChoseWithinBeam_tas(sDetectFrame* detClutProOut, sTrackingTarget* tasTrackStr, sMultiBeamMeas* cacheBeam, uint8_t tasBeamTotal)
//{
//    uint8_t n_det, Idx_maxCur, i_clusObj = 0;
//    uint8_t numTAS;
//    uint8_t numPoints = 0;
//    uint8_t trackDetNum;
//    float tmpMag, Mag_maxCur = 0;
//    uint8_t clusterID[MAX_NUM_DETECTS] = { 0 };
//    sDetTrackFrame* TrackDetStr = gTrackDetStr;//TAS目标DBScan聚类筛选的检测点结构体
//    sDetTrackFrame* cacheTas;
//
//    //**** 1、据TAS目标对当前帧检测点进行DBScan聚类筛选 ****//
//    trackDetNum = detTrack_DBscan(detClutProOut, tasTrackStr, clusterID);
//
//    memset(TrackDetStr, 0, sizeof(sDetTrackFrame));//波位内可与TAS目标聚类的检测结构体初始化
//
//    numPoints = detClutProOut->detectObjNum;//波位内的检测点总数
//
//#ifdef DEBUG_LOG_HXJ
//    my_printf("1111111111111111trackDetNum numPoints  [%d %d]\n", trackDetNum, numPoints);
//#endif
//
//    if (trackDetNum > 0)
//    {
//        for (i_clusObj = 0; i_clusObj < trackDetNum; i_clusObj++)
//        {
//            for (n_det = 0; n_det < numPoints; n_det++)
//            {
//                if (clusterID[n_det] == 1)
//                {
//                    tmpMag = detClutProOut->detPointsList[n_det].mag * INV_ONE6FORMAT_SCALE;
//                    if (tmpMag > Mag_maxCur)
//                    {
//                        Idx_maxCur = n_det;
//                        Mag_maxCur = tmpMag;
//                    }
//
//                    memcpy(&TrackDetStr->detectFrame.detPointsList[i_clusObj], &detClutProOut->detPointsList[n_det], sizeof(sDetectPoint));
//                    clusterID[n_det] = 0;
//                    break;
//                }
//            }
//        }
//        TrackDetStr->detectFrame.detectObjNum = trackDetNum;//波位内可与TAS目标聚类的检测点总数
//    }
//    TrackDetStr->detectFrame.detectObjByte = detClutProOut->detectObjByte;
//    TrackDetStr->detectFrame.frameID = detClutProOut->frameID;
//    TrackDetStr->detectFrame.timestamp = detClutProOut->timestamp;
//    TrackDetStr->detectFrame.trackTwsTasFlag = detClutProOut->trackTwsTasFlag;
//    TrackDetStr->aziBeamSin[0] = (int16_t)(sinf( detClutProOut->detPointsList[Idx_maxCur].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD) * ONE15FORMAT_SCALE);
//    TrackDetStr->eleBeamSin[0] = (int16_t)(sinf(detClutProOut->detPointsList[Idx_maxCur].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD) * ONE15FORMAT_SCALE);
//
//
//    //**** 2、波位信息存储 ****//
//    numTAS = cacheBeam->numTas;
//
//#ifdef DEBUG_LOG_HXJ
//    my_printf("================numTAS [%d]\n", numTAS);
//#endif
//
//    cacheTas = (sDetTrackFrame*)cacheBeam->multiBeam_tas;
//    if (numTAS < tasBeamTotal)
//    {
//        memcpy(&cacheTas[cacheBeam->numTas], TrackDetStr, sizeof(sDetTrackFrame));
//        cacheBeam->numTas++;
//    }
//    else
//    {
//        //集齐4个波位后，直接清零，从0波位开始赋值
//        memset(cacheTas, 0, COCH_BEAM_NUM_TAS * sizeof(sDetTrackFrame));
//        cacheBeam->numTas = numTAS - tasBeamTotal;
//        memcpy(&cacheTas[cacheBeam->numTas], TrackDetStr, sizeof(sDetTrackFrame));
//        cacheBeam->numTas++;
//    }
//
//}
//
////**** TAS模式-波位间点迹聚类处理模块 ****//
//void detClusteringBetweenBeam_tas(sDetTrackFrame* TrackDetStr, sTrackingTarget* tasTrackStr, uint8_t* cacheBeamTarNum, uint8_t tasBeamTotal, sMeasOutput* measObjOut)
//    {
//        uint8_t n_obj, i_tas, i_beamDet;
//        uint8_t i_det, add;
//        uint8_t ifCalcAzi, ifCalcEle;
//        uint8_t IdxBeam[COCH_BEAM_NUM_TAS] = { 0 };
//        uint16_t trackDetNum;
//        float tmpMag, tmpRange, tmpDoppler,tmpAzi,tmpEle, sumMag = 0;
//        float K, Mag, Range, Doppler, weightAzi = 0.f, weightEle = 0.f;
//        float MagBeam[COCH_BEAM_NUM_TAS] = { 0.0f };
//        float phiBeam[COCH_BEAM_NUM_TAS / 2] = { 0.0f };
//        float thetaBeam[COCH_BEAM_NUM_TAS / 2] = { 0.0f };
//        float theta, phi;
//        float differSumRatio_azi = 0, differSumRatio_ele = 0;
//        sDetectFrame* detInst;
//        detInst = &TrackDetStr->detectFrame;
//
//        trackDetNum = detInst->detectObjNum;//4个TAS波位内总的检测点数目        
//
//        if (trackDetNum < 1)
//        {
//            return;
//        }
//        else
//        { 
//            //**** 点迹聚类输出：TAS跟踪目标+搜索多波位内可聚类检测点 ****//
//            n_obj = 0;
//            add = 0;
//            Mag = 0;
//            Range = 0;
//            Doppler = 0;
//            sumMag = 0;
//
//            memset(IdxBeam, CLUSTER_POINT_LONELY, COCH_BEAM_NUM_TAS * sizeof(uint8_t));
//            memset(MagBeam, 0, COCH_BEAM_NUM_TAS * sizeof(float));
//
//            //依次遍历4个波位：下上左右
//            for (i_tas = 0; i_tas < COCH_BEAM_NUM_TAS; i_tas++)
//            {
//                for (i_beamDet = 0; i_beamDet < cacheBeamTarNum[i_tas]; i_beamDet++)
//                {
//                    i_det = add + i_beamDet;
//
//                    tmpMag = (float)(detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE);// *INV_ONE6FORMAT_SCALE;
//                    sumMag = sumMag + tmpMag;
//
//                    if (tmpMag > MagBeam[i_tas])
//                    {
//                        IdxBeam[i_tas] = i_det;
//                        MagBeam[i_tas] = tmpMag;
//                    }
//                }
//                add = add + cacheBeamTarNum[i_tas];
//            }        
//
//            //聚类目标range ,doppler mag计算
//
//            for (i_det = 0; i_det < trackDetNum; i_det++)
//            {
//                tmpMag = (float)(detInst->detPointsList[i_det].mag * INV_ONE6FORMAT_SCALE);// *INV_ONE6FORMAT_SCALE;
//                tmpRange = (float)(detInst->detPointsList[i_det].range * INV_ONE6FORMAT_SCALE);// * INV_ONE6FORMAT_SCALE;
//                tmpDoppler = (float)(detInst->detPointsList[i_det].velocity * INV_ONE6FORMAT_SCALE);// * INV_ONE6FORMAT_SCALE;
//                tmpAzi = detInst->detPointsList[i_det].azimuth*INV_ONE6FORMAT_SCALE;
//                tmpEle = detInst->detPointsList[i_det].elevation*INV_ONE6FORMAT_SCALE;
//                K = tmpMag / sumMag;
//                Mag = Mag + tmpMag * K;
//                Range = Range + tmpRange * K;
//                Doppler = Doppler + tmpDoppler * K;
//                weightAzi = weightAzi + tmpAzi * K;
//                weightEle = weightEle + tmpEle * K;
//                measObjOut->measurement[n_obj].detectionId[i_det] = detInst->detPointsList[i_det].id;//所有TAS波位内各聚类目标包含的点迹ID
//            }
//			/** init pitch and azimuth of plot clotting */
//			phi = weightEle * DEG2RAD;
//			theta = weightAzi * DEG2RAD;
//            //比幅测角：2维
//            ifCalcEle = 1;
//            for (i_tas = 0; i_tas < COCH_BEAM_NUM_TAS / 2; i_tas++)
//            {
//                if (IdxBeam[i_tas] != CLUSTER_POINT_LONELY)
//                {
//                    phiBeam[i_tas] = (float)(asinf(TrackDetStr->eleBeamSin[i_tas] * INV_ONE15FORMAT_SCALE));//下方/上方波位
//                }
//                else
//                {
//                    ifCalcEle = 0;
//                    break;
//                }
//            }
//
//            ifCalcAzi = 1;
//            for (i_tas = COCH_BEAM_NUM_TAS / 2; i_tas < COCH_BEAM_NUM_TAS; i_tas++)
//            {
//                if (IdxBeam[i_tas] != CLUSTER_POINT_LONELY)
//                {
//                    thetaBeam[i_tas - COCH_BEAM_NUM_TAS / 2] = (float)(asinf(TrackDetStr->aziBeamSin[i_tas] * INV_ONE15FORMAT_SCALE));//左方/右方波位
//                }
//                else
//                {
//                    ifCalcAzi = 0;
//                    break;
//                }
//            }
//        
//            //测俯仰角 
//            if ((MagBeam[0] != 0) && (MagBeam[1] != 0))
//            {
//                differSumRatio_ele = fabsf((MagBeam[0] - MagBeam[1]) / (MagBeam[0] + MagBeam[1]));
//                if ((ifCalcEle == 1) && (differSumRatio_ele <= DIFFER_SUM_RATIO_THRESHOLD_ELE))
//                {
//                    phi = ((phiBeam[0] + phiBeam[1]) / 2) + (K_ELE_TWS * cosf(phiBeam[1])) * ((MagBeam[0] - MagBeam[1]) / (MagBeam[0] + MagBeam[1]));//L-R,thetaL = theta1,thetaBeam=(theta1 + theta2) / 2
//                }
//            }
//            //测方位角
//            if ((MagBeam[2] != 0) && (MagBeam[3] != 0))
//            {
//                differSumRatio_azi = fabsf((MagBeam[2] - MagBeam[3]) / (MagBeam[2] + MagBeam[3]));
//                if ((ifCalcAzi == 1) && (differSumRatio_azi <= DIFFER_SUM_RATIO_THRESHOLD_AZI))
//                {
//                    theta = ((thetaBeam[0] + thetaBeam[1]) / 2) + (K_AZI_TWS * cosf(thetaBeam[1])) * ((MagBeam[2] - MagBeam[3]) / (MagBeam[2] + MagBeam[3]));//L-R,thetaL = theta1,thetaBeam=(theta1 + theta2) / 2
//                }
//            }
//            //TAS聚类目标赋值
//            measObjOut->num = 1;//仅处理输出TAS搜索的跟踪目标
//            measObjOut->measurement[n_obj].detId = n_obj;
//            measObjOut->measurement[n_obj].mag = Mag;
//            measObjOut->measurement[n_obj].vector.range = Range;
//            measObjOut->measurement[n_obj].vector.doppler = Doppler;
//            measObjOut->measurement[n_obj].vector.azimuthRad = theta;
//            measObjOut->measurement[n_obj].vector.pitchRad = phi;
//            measObjOut->measurement[n_obj].x = Range * cosf(phi) * cosf(theta);
//            measObjOut->measurement[n_obj].y = Range * cosf(phi) * sinf(theta);
//            measObjOut->measurement[n_obj].z = Range * sinf(phi);
//            measObjOut->measurement[n_obj].sinAzim = sinf(theta);
//            measObjOut->measurement[n_obj].cosAzim = cosf(theta);
//            measObjOut->measurement[n_obj].aziBeamSin = TrackDetStr->aziBeamSin[tasBeamTotal - 1];//TAS当前最后帧波位角
//            measObjOut->measurement[n_obj].eleBeamSin = TrackDetStr->eleBeamSin[tasBeamTotal - 1];//TAS当前最后帧波位角
//            measObjOut->measurement[n_obj].detectionNum = trackDetNum;//聚类簇里面包含的点迹个数
//            measObjOut->measurement[n_obj].classification = 0; // TODO: input real value
//         }
//    }


////**** TWS模式-波位内点迹聚类处理函数 ****//
//void clusteringFunInBeam_tws(sClusterDetect* clusterDet, sDetectFrame* detClutProOut, sMeasOutput* clusterObjIn)
//{
//    uint8_t i_det, Idx_maxCur, i_clustObj, i_clustID, i_lonelyObj;
//    float tmpMag, tmpRange, tmpDoppler, Mag_maxCur;
//    float K, Mag, Range, Doppler, sumMag = 0 ;// unit/2^6
//    float theta, phi;
//    uint8_t *clustID = clusterDet->clusterID;
//    uint8_t clustNum = clusterDet->clusterNum;
//    uint8_t lonelyNum = clusterDet->lonelyNum;
//    uint16_t detNum = detClutProOut->detectObjNum;
//    sDetectPoint* clusterDetect = detClutProOut->detPointsList;
//
//    memset(clusterObjIn, 0, sizeof(sMeasOutput));
//
//#ifdef DEBUG_LOG_HXJ
//    //my_printf("==============clustID_before [%d %d %d %d]\n", clustID[0], clustID[1], clustID[2], clustID[3]);
//    //my_printf("aziSin eleSin [%.2f %.2f]\n", theta*RAD2DEG, phi*RAD2DEG);
//#endif // DEBUG_LOG_HXJ
//#ifdef DEBUG_LOG_HXJ
//    //my_printf("clustNum lonelyNum [%d %d]\n", clustNum, lonelyNum);
//#endif // DEBUG_LOG_HXJ
//    clusterObjIn->num = clustNum + lonelyNum;//点迹聚类后的目标总数
//
//    //(1)点迹聚类
//    for (i_clustObj = 0; i_clustObj < clustNum; i_clustObj++)
//    {
//        i_clustID = 0;
//        Mag = 0;
//        Range = 0;
//        Doppler = 0;
//        Mag_maxCur = 0;
//        sumMag = 0;
//
//
//        for (i_det = 0; i_det < detNum; i_det++)
//        {
//            if (clustID[i_det] == (i_clustObj + 1))
//            {
//                tmpMag = clusterDetect[i_det].mag * INV_ONE6FORMAT_SCALE;
//                sumMag = sumMag + tmpMag;
//
//                if (tmpMag > Mag_maxCur)
//                {
//                    Idx_maxCur = i_det;
//                    Mag_maxCur = tmpMag;
//                }
//
//                clusterObjIn->measurement[i_clustObj].detectionId[i_clustID] = clusterDetect[i_det].id;//i_det,各聚类目标包含的点迹ID，有问题？？？？？？？？？？？？？？？？？
//                i_clustID++;
//            }
//        }
//        clusterObjIn->measurement[i_clustObj].detectionNum = i_clustID;//聚类簇里面包含的点迹个数
//
//        //**** 聚类目标赋值 ***//      
//        //波位内聚类目标角赋值幅值最大的聚类检测点角度信息
//        theta = (float)(clusterDetect[Idx_maxCur].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD);
//        phi = (float)(clusterDetect[Idx_maxCur].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD);
//
//        for (i_det = 0; i_det < detNum; i_det++)
//        {
//            if (clustID[i_det] == (i_clustObj + 1))
//            {
//
//                tmpMag = clusterDetect[i_det].mag * INV_ONE6FORMAT_SCALE;
//                tmpRange = clusterDetect[i_det].range * INV_ONE6FORMAT_SCALE;
//                tmpDoppler = clusterDetect[i_det].velocity * INV_ONE6FORMAT_SCALE;
//
//                K = tmpMag / sumMag;
//                Mag = Mag + tmpMag * K;
//                Range = Range + tmpRange * K;
//                Doppler = Doppler + tmpDoppler * K;  
//
//            }
//        }
//        clusterObjIn->measurement[i_clustObj].detId = i_clustObj;//点迹聚类后的目标ID
//        clusterObjIn->measurement[i_clustObj].mag = Mag;
//        clusterObjIn->measurement[i_clustObj].vector.range = Range;
//        clusterObjIn->measurement[i_clustObj].vector.doppler = Doppler;
//        clusterObjIn->measurement[i_clustObj].vector.azimuthRad = theta;
//        clusterObjIn->measurement[i_clustObj].vector.pitchRad = phi;
//        clusterObjIn->measurement[i_clustObj].x = Range * cosf(phi) * cosf(theta);
//        clusterObjIn->measurement[i_clustObj].y = Range * cosf(phi) * sinf(theta);
//        clusterObjIn->measurement[i_clustObj].z = Range * sinf(phi);
//        clusterObjIn->measurement[i_clustObj].sinAzim = sinf(theta);
//        clusterObjIn->measurement[i_clustObj].cosAzim = cosf(theta);
//        clusterObjIn->measurement[i_clustObj].aziBeamSin = (int16_t)(sinf(theta)* ONE15FORMAT_SCALE);
//        clusterObjIn->measurement[i_clustObj].eleBeamSin = (int16_t)(sinf(phi) * ONE15FORMAT_SCALE);
//        clusterObjIn->measurement[i_clustObj].classification = 0; // TODO: input real value
//    }
//
//    //(2)孤点目标输出
//    for (i_lonelyObj = 0; i_lonelyObj < lonelyNum; i_lonelyObj++)
//    {
//        for (i_det = 0; i_det < detNum; i_det++)
//        {
//            if (clustID[i_det] == CLUSTER_POINT_LONELY)
//            {
//                //波位内聚类目标角赋值幅值最大的聚类检测点角度信息
//                theta = (float)(clusterDetect[i_det].azimuth * INV_ONE6FORMAT_SCALE * DEG2RAD);
//                phi = (float)(clusterDetect[i_det].elevation * INV_ONE6FORMAT_SCALE * DEG2RAD);
//
//                clusterObjIn->measurement[clustNum + i_lonelyObj].detId = clustNum + i_lonelyObj;//点迹聚类后的目标ID
//                clusterObjIn->measurement[clustNum + i_lonelyObj].detectionId[0] = clusterDetect[i_det].id;//i_det;各聚类目标包含的点迹ID
//                clusterObjIn->measurement[clustNum + i_lonelyObj].detectionNum = 1;//聚类簇里面包含的点迹个数
//                clusterObjIn->measurement[clustNum + i_lonelyObj].mag = clusterDetect[i_det].mag * INV_ONE6FORMAT_SCALE;
//                clusterObjIn->measurement[clustNum + i_lonelyObj].vector.range = clusterDetect[i_det].range * INV_ONE6FORMAT_SCALE;
//                clusterObjIn->measurement[clustNum + i_lonelyObj].vector.doppler = clusterDetect[i_det].velocity * INV_ONE6FORMAT_SCALE;
//                clusterObjIn->measurement[clustNum + i_lonelyObj].vector.azimuthRad = theta;
//                clusterObjIn->measurement[clustNum + i_lonelyObj].vector.pitchRad = phi;
//                clusterObjIn->measurement[clustNum + i_lonelyObj].x = (clusterDetect[i_det].range * INV_ONE6FORMAT_SCALE) * cosf(phi) * cosf(theta);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].y = (clusterDetect[i_det].range * INV_ONE6FORMAT_SCALE) * cosf(phi) * sinf(theta);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].z = (clusterDetect[i_det].range * INV_ONE6FORMAT_SCALE) * sinf(phi);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].sinAzim = sinf(theta);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].cosAzim = cosf(theta);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].aziBeamSin = (int16_t)(sinf(theta) * ONE15FORMAT_SCALE);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].eleBeamSin = (int16_t)(sinf(phi) * ONE15FORMAT_SCALE);
//                clusterObjIn->measurement[clustNum + i_lonelyObj].classification = 0; //TODO: input real value
//                    
//                clustID[i_det] = clustNum + i_lonelyObj + 1;
////#ifdef DEBUG_LOG_HXJ
////                    my_printf("!!!!!!!!!!!!!!!!!!clustID_after [%d %d %d %d]\n", clustID[0], clustID[1], clustID[2], clustID[3]);
////                    //my_printf("aziSin eleSin [%.2f %.2f]\n", theta*RAD2DEG, phi*RAD2DEG);
////#endif // DEBUG_LOG_HXJ
//                break;
//            }
//        }
//
//    }
//
//}


////**** TWS模式-波位间点迹聚类处理函数 ****//
//void clusteringFunBetweenBeams(uint8_t N_curObj, uint8_t N_cacheObj, uint8_t calcAngleMode, sClusterMeasure* clusterMeas, sMeasOutput* measObjOut)
//    {
//        uint8_t i_curObj, i_clustObj, i_cacheObj, i_lonelyObj;
//        uint8_t Idx_maxCur, Idx_maxCache;
//        uint8_t clustNum, lonelyNum, tmpDetNum = 0;
//        uint8_t clusterMeasNum;//clusterMeasNum = clustNum + lonelyNum
//        float tmpMag, tmpRange, tmpDoppler;
//        float K, Mag, Range, Doppler;// 2^6
//        float Mag_maxCur = 0, Mag_maxCache = 0, sumMag = 0, differSumRatio = 0; //maxMag
//        float theta_Cur, phi_Cur, theta_Cache, phi_Cache, theta, phi;
//        sMeasurement_SphPoint* inMeas;//*outMeas, 波位间聚类目标缓存结构体
//
//        clustNum = clusterMeas->clusterNum;
//        lonelyNum = clusterMeas->lonelyNum;
//        clusterMeasNum = clusterMeas->clusterMeasNum;//clusterMeasNum = N_curObj+N_cacheObj
//
//        /* 输入结构体 */
//        inMeas = clusterMeas->measurement;
//
//        memset(measObjOut, 0, sizeof(sMeasOutput));
//
//        //波位间聚类输出状态信息为当前波位状态信息
//        measObjOut->num = clustNum + lonelyNum;//点迹聚类后的目标总数
//
//
//        //(1)点迹聚类
//        for (i_clustObj = 0; i_clustObj < clustNum; i_clustObj++)
//        {
//            Mag = 0;
//            Range = 0;
//            Doppler = 0;
//            sumMag = 0;
//
//            Idx_maxCur = 0;
//            Mag_maxCur = 0;
//            Idx_maxCache = N_curObj;
//            Mag_maxCache = 0;
//
//            tmpDetNum = 0;
//            differSumRatio = 0;
//
//            for (i_curObj = 0; i_curObj < N_curObj; i_curObj++)
//            {
//                if (clusterMeas->clusterID[i_curObj] == (i_clustObj + 1))
//                {
//                    tmpMag = inMeas[i_curObj].mag;// * INV_ONE6FORMAT_SCALE;
//                    sumMag = sumMag + tmpMag;
//
//                    if (tmpMag > Mag_maxCur)
//                    {
//                        Idx_maxCur = i_curObj;
//                        Mag_maxCur = tmpMag;
//                    }
//                    
//                    memcpy(&measObjOut->measurement[i_clustObj].detectionId[tmpDetNum], inMeas[i_curObj].detectionId, inMeas[i_curObj].detectionNum * sizeof(uint16_t));//当前最新一帧各聚类目标包含的点迹ID
//                    tmpDetNum = tmpDetNum + inMeas[i_curObj].detectionNum;//聚类簇里面包含的点迹个数
//                }
//            }
//            measObjOut->measurement[i_clustObj].detectionNum = tmpDetNum;//聚类簇里面包含的点迹个数
//
//            for (i_cacheObj = 0; i_cacheObj < N_cacheObj; i_cacheObj++)
//            {
//                if (clusterMeas->clusterID[N_curObj + i_cacheObj] == (i_clustObj + 1))
//                {
//                    tmpMag = inMeas[N_curObj + i_cacheObj].mag;// *INV_ONE6FORMAT_SCALE;
//                    sumMag = sumMag + tmpMag;
//
//                    if (tmpMag > Mag_maxCache)
//                    {
//                        Idx_maxCache = N_curObj + i_cacheObj;
//                        Mag_maxCache = tmpMag;
//                    }
//                }
//            }
//
//            //聚类目标range ,doppler mag计算
//            for (i_curObj = 0; i_curObj < clusterMeasNum; i_curObj++)
//            {
//                if (clusterMeas->clusterID[i_curObj] == (i_clustObj + 1))
//                {
//                    tmpMag = inMeas[i_curObj].mag;// *INV_ONE6FORMAT_SCALE;
//                    tmpRange = inMeas[i_curObj].vector.range;// * INV_ONE6FORMAT_SCALE;
//                    tmpDoppler = inMeas[i_curObj].vector.doppler;// * INV_ONE6FORMAT_SCALE;
//
//                    K = tmpMag / sumMag;
//                    Mag = Mag + tmpMag * K;
//                    Range = Range + tmpRange * K;
//                    Doppler = Doppler + tmpDoppler * K;
//                }
//            }
//
//            //比幅测角：1为当前波位，2为缓存波位,相扫信息
//            theta_Cur = (float)(asinf(inMeas[Idx_maxCur].aziBeamSin * INV_ONE15FORMAT_SCALE)); //为当前波位,若无聚类点，Idx_maxCur=0为当前波位值
//            phi_Cur = (float)(asinf(inMeas[Idx_maxCur].eleBeamSin * INV_ONE15FORMAT_SCALE));
//
//            theta_Cache = (float)(asinf(inMeas[Idx_maxCache].aziBeamSin * INV_ONE15FORMAT_SCALE));//为缓存波位
//            phi_Cache = (float)(asinf(inMeas[Idx_maxCache].eleBeamSin * INV_ONE15FORMAT_SCALE));
//
//            //聚类后目标先赋值当前状态测得角度
//            theta = inMeas[Idx_maxCur].vector.azimuthRad;
//            phi = inMeas[Idx_maxCur].vector.pitchRad;
//
//            //满足条件，进入测角处理，更新对应角度值
//            if ((Mag_maxCur != 0)&& (Mag_maxCache != 0))
//            { 
//                differSumRatio = fabsf(Mag_maxCur - Mag_maxCache) / (Mag_maxCur + Mag_maxCache);
//                if ((calcAngleMode == 1) && (differSumRatio <= DIFFER_SUM_RATIO_THRESHOLD_AZI))//测方位角
//                {
//
//                    if (theta_Cur < theta_Cache)
//                    {
//                        theta = (theta_Cur + theta_Cache) / 2 + (K_AZI_TWS * cosf(theta_Cache)) * (Mag_maxCur - Mag_maxCache) / (Mag_maxCur + Mag_maxCache);//L-R,thetaL = theta_Cur,theta0=(theta_Cur + theta_Cache) / 2
//                    }
//                    else
//                    {
//                        theta = (theta_Cur + theta_Cache) / 2 + (K_AZI_TWS * cosf(theta_Cur)) * (Mag_maxCache - Mag_maxCur) / (Mag_maxCur + Mag_maxCache);//thetaL = theta_Cache
//                    }
//                }
//                else if ((calcAngleMode == 2) && (differSumRatio <= DIFFER_SUM_RATIO_THRESHOLD_ELE))//测俯仰角
//                {
//                    if (phi_Cur < phi_Cache)
//                    {
//                        phi = (phi_Cur + phi_Cache) / 2 + (K_ELE_TWS * cosf(phi_Cache)) * (Mag_maxCur - Mag_maxCache) / (Mag_maxCur + Mag_maxCache);//phiL = phi_Cur
//                    }
//                    else
//                    {
//                        phi = (phi_Cur + phi_Cache) / 2 + (K_ELE_TWS * cosf(phi_Cur)) * (Mag_maxCache - Mag_maxCur) / (Mag_maxCur + Mag_maxCache);//phiL = phi_Cache
//                    }
//                }
//            }
//           
//            //聚类目标赋值
//            measObjOut->measurement[i_clustObj].detId = i_clustObj;
//            measObjOut->measurement[i_clustObj].mag = Mag;
//            measObjOut->measurement[i_clustObj].vector.range = Range;
//            measObjOut->measurement[i_clustObj].vector.doppler = Doppler;
//            measObjOut->measurement[i_clustObj].vector.azimuthRad = theta;
//            measObjOut->measurement[i_clustObj].vector.pitchRad = phi;
//            measObjOut->measurement[i_clustObj].x = Range * cosf(phi) * cosf(theta);
//            measObjOut->measurement[i_clustObj].y = Range * cosf(phi) * sinf(theta);
//            measObjOut->measurement[i_clustObj].z = Range * sinf(phi);
//            measObjOut->measurement[i_clustObj].sinAzim = sinf(theta);
//            measObjOut->measurement[i_clustObj].cosAzim = cosf(theta);
//            measObjOut->measurement[i_clustObj].aziBeamSin = inMeas[Idx_maxCur].aziBeamSin;
//            measObjOut->measurement[i_clustObj].eleBeamSin = inMeas[Idx_maxCur].eleBeamSin;
//            measObjOut->measurement[i_clustObj].classification = 0; //TODO: input real value
//        }
//
//        //(2)孤点目标输出
//        for (i_lonelyObj = 0; i_lonelyObj < lonelyNum; i_lonelyObj++)
//        {
//            for (i_curObj = 0; i_curObj < N_curObj; i_curObj++)
//            {
//                if (clusterMeas->clusterID[i_curObj] == CLUSTER_POINT_LONELY)
//                {
//                    memcpy(&measObjOut->measurement[clustNum + i_lonelyObj], &inMeas[i_curObj], sizeof(sMeasurement_SphPoint));
//                    measObjOut->measurement[clustNum + i_lonelyObj].detId = clustNum + i_lonelyObj;//点迹聚类后的目标ID
//                    measObjOut->measurement[clustNum + i_lonelyObj].classification = 0; //TODO: input real value
//                    clusterMeas->clusterID[i_curObj] = clustNum + i_lonelyObj + 1;
//                    break;
//                }
//            }
//
//        }
//
//    }



////**** TWS模式-DBScan遍历当前波位+存储波位进行聚类子函数 ****//
//void cluster_DBscan(sClusterMeasure* clusterMeas, uint8_t N)
//    {
//        uint8_t n_det, i_current, i_neigh;
//        uint8_t* neighLast;
//        uint8_t* neighCurrent;
//        uint8_t neighCount, newCount;
//        uint8_t clustTarID = 0, lonelyTarID = 0;
//        uint8_t clusterMeasNum;
//        uint8_t scope[MAX_NUM_DETECTS] = { 0 };
//        uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };
//        uint8_t visited[MAX_NUM_DETECTS] = { 0 };
//
//        clusterMeasNum = clusterMeas->clusterMeasNum;
//
//        memset(visited, CLUSTER_POINT_UNKNOWN, clusterMeasNum * sizeof(uint8_t));
//        memset(clusterMeas->clusterID, CLUSTER_POINT_LONELY, clusterMeasNum * sizeof(uint8_t));
//
//
//        // scan through all the points to find its neighbors
//        for (n_det = 0; n_det < clusterMeasNum; n_det++)
//        {
//            if (visited[n_det] != CLUSTER_POINT_VISITED)
//            {
//                visited[n_det] = CLUSTER_POINT_VISITED;
//
//                neighCurrent = neighLast = neighbors;
//                // scope is the local copy of visit
//                memcpy(scope, visited, clusterMeasNum * sizeof(uint8_t));
//
//                neighCount = dot_store_DBscan_findNeighbors(
//                    clusterMeas->measurement, n_det, neighLast, clusterMeasNum,
//                    scope, &newCount);
//
//#ifdef DEBUG_LOG_HXJ
//                //my_printf("det[%d]: neightCount: %d\n", n_det, neighCount);
//#endif
//                /* The cluster consists of the point itself and its neighbours. */
//                if (neighCount < MIN_NUM_POINTS_IN_CLUSTER - 1)
//                {
//                    // This point is lonely point
//                    clusterMeas->clusterID[n_det] = CLUSTER_POINT_LONELY;
//                }
//                else
//                {
//                    // This point belongs to a New Cluster
//                    clustTarID++;                                // New cluster ID
//                    clusterMeas->clusterID[n_det] = clustTarID;      // This point belong to this cluster
//
//                    // tag all the neighbors as visited_between in scope so that it will not be found again when searching neighbor's neighbor.
//                    for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//                    {
//                        i_current = neighLast[i_neigh];//与当前n_det聚类的ID
//                        scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
//                    }
//                    neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位
//
//                    while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
//                    {
//                        // Explore the neighborhood
//                        i_current = *neighCurrent++;               // Take point from the neighborhood
//                        clusterMeas->clusterID[i_current] = clustTarID; // All points from the neighborhood also belong to this cluster
//                        visited[i_current] = CLUSTER_POINT_VISITED;
//
//                        neighCount = dot_store_DBscan_findNeighbors(
//                            clusterMeas->measurement, i_current, neighLast, clusterMeasNum,
//                            scope, &newCount);
//
//                        if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
//                        {
//                            for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//                            {
//                                i_current = neighLast[i_neigh];
//                                scope[i_current] = CLUSTER_POINT_VISITED;
//                            }
//                            neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
//                        }
//                    }
//                    if (clustTarID >= MAX_NUM_DETECTS)
//                    {
//                        return;
//                    }
//                }
//            }
//        }
//
//        clusterMeas->clusterNum = clustTarID;//可聚类目标数
//
//         //只对TAS模式扫描的4波位内目标进行孤点查找输出
//        for (n_det = 0; n_det < N; n_det++)
//        {
//            if (clusterMeas->clusterID[n_det] == CLUSTER_POINT_LONELY)
//            {
//                lonelyTarID = lonelyTarID + 1;
//            }
//        }
//
//        clusterMeas->lonelyNum = lonelyTarID;//孤点目标
//
//#ifdef DEBUG_LOG_HXJ
//        //my_printf("numPoints clustTarID lonelyTarID [%d %d %d]\n", clusterMeasNum, clustTarID, lonelyTarID);
//#endif // DEBUG_LOG_HXJ
//
//    }

////**** TAS模式-DBScan遍历当前波位+跟踪目标进行聚类子函数 ****//
//uint8_t detTrack_DBscan(sDetectFrame* detClutProOut, sTrackingTarget* tasTrackStr, uint8_t *clusterID)
//    {
//        uint8_t n_det, i_current, i_neigh;
//        uint8_t numPoints = 0;
//        uint8_t clustTarID = 0;
//        uint8_t* neighLast;
//        uint8_t* neighCurrent;
//        uint8_t neighCount, newCount;
//        uint8_t visited_in[MAX_NUM_DETECTS] = { 0 };
//        uint8_t scope[MAX_NUM_DETECTS] = { 0 };
//        uint8_t neighbors[MAX_NUM_DETECTS] = { 0 };
//        sDetectPoint* detPointStr;// 
//        uint8_t trackDetNum = 0;//可与TAS跟踪目标聚类的检测点总数
//
//        //**** 1、目标属性归类 ****//
//        numPoints = detClutProOut->detectObjNum;
//        detPointStr = detClutProOut->detPointsList;
//        memset(visited_in, CLUSTER_POINT_UNKNOWN, numPoints * sizeof(uint8_t));
//
//        // Init the clusterID of points are CLUSTER_POINT_LONELY
//        for (n_det = 0; n_det < numPoints; n_det++)
//        {
//            clusterID[n_det] = CLUSTER_POINT_LONELY;
//        }
//        // scan through all the points to find its neighbors
//        neighCurrent = neighLast = neighbors;
//        // scope is the local copy of visit
//        memcpy(scope, visited_in, numPoints * sizeof(uint8_t));
//
//        neighCount = dot_tracker_DBscan_findNeighbors(
//            detPointStr, tasTrackStr, neighLast, numPoints,
//            scope, &newCount, detClutProOut->waveType);
//
//        trackDetNum += neighCount;
//
//        /* The cluster consists of the point itself and its neighbours. */
//        if (neighCount < 1)
//        {
//            // This point is lonely point
//            return trackDetNum;
//        }
//        else
//        {            
//            // This point belongs to a New Cluster
//            clustTarID = 1;                                // New cluster ID，current tracking target
//
//            // tag all the neighbors as visited_in in scope so that it will not be found again when searching neighbor's neighbor.
//            for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//            {
//                i_current = neighLast[i_neigh];//可与当前TAS目标聚类的ID
//                scope[i_current] = CLUSTER_POINT_VISITED;//与当前n_det聚类的ID打标签
//            }
//            neighLast += newCount;//neighLast后移，此次遍历后聚类的目标数，位
//
//            while (neighCurrent != neighLast)               // neigh shall be at least minPoints in front of neighborhood pointer
//            {
//                // Explore the neighborhood
//                i_current = *neighCurrent++;               // Take point from the neighborhood
//                clusterID[i_current] = clustTarID; // All points from the neighborhood also belong to this cluster
//                visited_in[i_current] = CLUSTER_POINT_VISITED;
//
//                neighCount = dot_dot_DBscan_findNeighbors(
//                    detPointStr, i_current, neighLast, numPoints,
//                    scope, &newCount);
//
//                if (neighCount >= MIN_NUM_POINTS_IN_CLUSTER - 1)
//                {
//                    for (i_neigh = 0; i_neigh < newCount; i_neigh++)
//                    {
//                        i_current = neighLast[i_neigh];
//                        scope[i_current] = CLUSTER_POINT_VISITED;
//                    }
//                    neighLast += newCount;              /* Member is a core point, and its neighborhood is added to the cluster */
//                }
//
//                trackDetNum += neighCount;
//            }
//
//        }
//        return trackDetNum;
//    }


////**** TWS模式-波位内DBScan点迹聚类子函数 ****//
//uint8_t dot_dot_DBscan_findNeighbors(sDetectPoint* detPointStr, uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_in, uint8_t* newCount)
//    {
//        uint8_t ii;
//        uint8_t newCounttmp = 0;
//        float tmpRange = 0, tmpVelocity = 0;
//        float rangeN = detPointStr[n].range * INV_ONE6FORMAT_SCALE;
//        float velocityN = detPointStr[n].velocity * INV_ONE6FORMAT_SCALE;
//        float rangeThreshold, veloThreshold;
//
//        rangeThreshold = RANGE_THRESHOLD_WITHIN;
//        veloThreshold = VELOCITY_THRESHOLD_WITHIN;
//
//        for (ii = 0; ii < numPoints; ii++)
//        {
//            if (visited_in[ii] == CLUSTER_POINT_VISITED)
//            {
//                continue;
//            }  
//
//            tmpRange = (float)(detPointStr[ii].range * INV_ONE6FORMAT_SCALE);
//            tmpVelocity = (float)(detPointStr[ii].velocity * INV_ONE6FORMAT_SCALE);
//            if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
//            {                                      
//                /* Mark this point as a neighbour in the list of
//                 * neighbours. Also increment the number of neighbours
//                 * for this point. */
//                *neigh = ii;
//                neigh++;
//                newCounttmp++;
//            }
//        }
//        *newCount = (uint16_t)newCounttmp;
//        return newCounttmp;
//    }

////**** TWS模式-波位间DBScan点迹聚类子函数 ****//
//uint8_t dot_store_DBscan_findNeighbors(sMeasurement_SphPoint* measPointStr, uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_between, uint8_t* newCount)
//    {
//        uint8_t ii;
//        uint8_t newCounttmp = 0;
//        float tmpRange = 0, tmpVelocity = 0;
//        float rangeN = measPointStr[n].vector.range;
//        float velocityN = measPointStr[n].vector.doppler;
//        float rangeThreshold, veloThreshold;
//
//        rangeThreshold = RANGE_THRESHOLD_BETWEEN;
//        veloThreshold = VELOCITY_THRESHOLD_BETWEEN;
//
//        for (ii = 0; ii < numPoints; ii++)
//        {
//            if (visited_between[ii] == CLUSTER_POINT_VISITED)
//            {
//                continue;
//            }
//
//            tmpRange = measPointStr[ii].vector.range;
//            tmpVelocity = measPointStr[ii].vector.doppler;
//            if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
//            {
//                /* Mark this point as a neighbour in the list of
//                 * neighbours. Also increment the number of neighbours
//                 * for this point. */
//                *neigh = ii;
//                neigh++;
//                newCounttmp++;
//            }
//        }
//        *newCount = (uint16_t)newCounttmp;
//        return newCounttmp;
//    }


////**** TAS模式-跟踪目标+检测点DBScan点迹聚类子函数 ****//
//uint8_t dot_tracker_DBscan_findNeighbors(sDetectPoint* detPointStr, sTrackingTarget* tasTrackStr, uint8_t* neigh,\
//	uint8_t numPoints, uint8_t* visited_in, uint8_t* newCount, sMeasurement_enum_wavetype waveType)
//    {
//        uint8_t ii;
//        uint8_t newCounttmp = 0;
//        float tmpRange = 0, tmpVelocity = 0;
//        float rangeN = tasTrackStr->range * INV_ONE6FORMAT_SCALE;
//        float velocityN = tasTrackStr->velocity * INV_ONE6FORMAT_SCALE;
//        float rangeThreshold, veloThreshold;
//
//        rangeThreshold = RANGE_THRESHOLD_TAS;
//        veloThreshold = VELOCITY_THRESHOLD_TAS;
//#ifdef VELOCITY_UNAMBIGIOUS
//		if (tasTrackStr->stateType == UNSTABLE_TRAJ)
//		{
//			/** TODO: velocity unambigious */
//			veloThreshold = MAXVEL;
//		}
//#endif // VELOCITY_UNAMBIGIOUS
//        for (ii = 0; ii < numPoints; ii++)
//        {
//            if (visited_in[ii] == CLUSTER_POINT_VISITED)
//            {
//                continue;
//            }
//            tmpRange = (float)(detPointStr[ii].range * INV_ONE6FORMAT_SCALE);
//            tmpVelocity = (float)(detPointStr[ii].velocity * INV_ONE6FORMAT_SCALE);
//#ifdef DEBUG_LOG_ZQ
//			my_printf("tas dbscan vmeas %.2f ", tmpVelocity);
//#endif // DEBUG_LOG_ZQ
//#ifdef VELOCITY_UNAMBIGIOUS
//			tmpVelocity = tracking_velocityUnambigious(waveType, velocityN, tmpVelocity);
//#endif // VELOCITY_UNAMBIGIOUS
//#ifdef DEBUG_LOG_ZQ
//			my_printf("tas dbscan vmeas %.2f vtrk %.2f dr dv [%.2f %.2f]", tmpVelocity, velocityN, tmpRange - rangeN, tmpVelocity - velocityN);
//#endif // DEBUG_LOG_ZQ
//
//            if ((fabsf(tmpRange - rangeN) <= rangeThreshold) && (fabsf(tmpVelocity - velocityN) <= veloThreshold))
//            {
//                /* Mark this point as a neighbour in the list of
//                 * neighbours. Also increment the number of neighbours
//                 * for this point. */
//                *neigh = ii;
//                neigh++;
//                newCounttmp++;
//            }
//        }
//        *newCount = (uint16_t)newCounttmp;
//        return newCounttmp;
//    }


//=============================== old =================================//
