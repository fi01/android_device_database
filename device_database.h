typedef enum {
  DEVICE_NOT_SUPPORTED = 0,
  DEVICE_F10D_V21R48A,
  DEVICE_F11D_V24R40A,
  DEVICE_HTL21_1_29_970_1,
  DEVICE_HTL22_1_05_970_1,
  DEVICE_HTL22_1_07_970_4,
  DEVICE_HTX21_1_20_971_1,
  DEVICE_IS17SH_01_00_04,
  DEVICE_ISW12K_010_0_3000,
  DEVICE_ISW13F_V69R51I,
  DEVICE_L01D_V20d,
  DEVICE_L02E_V10c,
  DEVICE_L02E_V10e,
  DEVICE_L06D_V10k,
  DEVICE_LT26W_1265_3909_6_2_B_0_200,
  DEVICE_LT26I_1257_8080_6_2_B_0_211,
  DEVICE_C6603_1269_5309_10_1_1_A_1_307,
  DEVICE_C6603_1275_1562_10_1_1_A_1_253,
  DEVICE_C5302_1272_1092_12_0_A_1_284,
  DEVICE_N05E_A1000311,
  DEVICE_SC04E_OMUAMDI,
  DEVICE_SCL21_KDALJD,
  DEVICE_SH04E_01_00_02,
  DEVICE_SH04E_01_00_03,
  DEVICE_SO01E_9_1_C_0_473,
  DEVICE_SO04D_7_0_D_1_137,
  DEVICE_SO05D_7_0_D_1_137,
  DEVICE_SOL21_9_1_D_0_395,
  DEVICE_SONYTABLET_P_RELEASE5A,
  DEVICE_SONYTABLET_S_RELEASE5A,
  DEVICE_LG_E975,
} device_id_t;

extern device_id_t detect_device(void);
extern void print_reason_device_not_supported(void);
