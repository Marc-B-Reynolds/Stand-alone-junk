const range_result_t test_vector[] =
{
  {{-0x1.242396p-22f,0x1.6f71a0p-24f},0x1.bb67aep-1f}, // ULPs of range
  {{-0x1.fffffep-1f,-0x1.fffffep-1f}, 0x1.000d10p-1f}, //     0  
  {{-0x1.fffff4p-1f,-0x1.fffff4p-1f}, 0x1.002000p-1f}, //     0  
  {{-0x1.ffffe6p-1f,-0x1.ffffe6p-1f}, 0x1.002f18p-1f}, //     0  
  {{-0x1.ffffd0p-1f,-0x1.ffffd0p-1f}, 0x1.003ffep-1f}, //     0  
  {{-0x1.ffffb6p-1f,-0x1.ffffb6p-1f}, 0x1.004f72p-1f}, //     0  
  {{-0x1.ffff94p-1f,-0x1.ffff94p-1f}, 0x1.005ffap-1f}, //     0  
  {{-0x1.ffff6ep-1f,-0x1.ffff6ep-1f}, 0x1.006f96p-1f}, //     0  
  {{-0x1.ffff40p-1f,-0x1.ffff40p-1f}, 0x1.007ff6p-1f}, //     0  
  {{-0x1.ffff0ep-1f,-0x1.ffff0ep-1f}, 0x1.008fa6p-1f}, //     0  
  {{-0x1.fffed4p-1f,-0x1.fffed4p-1f}, 0x1.009ff0p-1f}, //     0  
  {{-0x1.fffe96p-1f,-0x1.fffe96p-1f}, 0x1.00afaep-1f}, //     0  
  {{-0x1.fffe50p-1f,-0x1.fffe50p-1f}, 0x1.00bfe8p-1f}, //     0  
  {{-0x1.fffe06p-1f,-0x1.fffe06p-1f}, 0x1.00cfb0p-1f}, //     0  
  {{-0x1.fffdb4p-1f,-0x1.fffdb4p-1f}, 0x1.00dfe0p-1f}, //     0  
  {{-0x1.fffd5ep-1f,-0x1.fffd5ep-1f}, 0x1.00efaep-1f}, //     0  
  {{-0x1.fffd00p-1f,-0x1.fffd00p-1f}, 0x1.00ffd6p-1f}, //     0  
  {{-0x1.fffc9ep-1f,-0x1.fffc9ep-1f}, 0x1.010fa8p-1f}, //     0  
  {{-0x1.fffc32p-1f,-0x1.fffc32p-1f}, 0x1.012016p-1f}, //     0  
  {{-0x1.fffbc4p-1f,-0x1.fffbc4p-1f}, 0x1.012fe8p-1f}, //     0  
  {{-0x1.fffb4ep-1f,-0x1.fffb4ep-1f}, 0x1.014002p-1f}, //     0  
  {{-0x1.fffad4p-1f,-0x1.fffad4p-1f}, 0x1.014fd8p-1f}, //     0  
  {{-0x1.fffa52p-1f,-0x1.fffa52p-1f}, 0x1.015feep-1f}, //     0  
  {{-0x1.fff9ccp-1f,-0x1.fff9ccp-1f}, 0x1.016fc6p-1f}, //     0  
  {{-0x1.fff93ep-1f,-0x1.fff93ep-1f}, 0x1.017fd8p-1f}, //     0  
  {{-0x1.fff8aap-1f,-0x1.fff8aap-1f}, 0x1.018feap-1f}, //     0  
  {{-0x1.fff810p-1f,-0x1.fff810p-1f}, 0x1.019ff8p-1f}, //     0  
  {{-0x1.fff772p-1f,-0x1.fff772p-1f}, 0x1.01afd2p-1f}, //     0  
  {{-0x1.fff6ccp-1f,-0x1.fff6ccp-1f}, 0x1.01bfdep-1f}, //     0  
  {{-0x1.fff620p-1f,-0x1.fff620p-1f}, 0x1.01cfeap-1f}, //     0  
  {{-0x1.fff56ep-1f,-0x1.fff56ep-1f}, 0x1.01dff2p-1f}, //     0  
  {{-0x1.fff4b8p-1f,-0x1.fff4b8p-1f}, 0x1.01efcep-1f}, //     0  
  {{-0x1.fff3fap-1f,-0x1.fff3fap-1f}, 0x1.01ffd6p-1f}, //     0  
  {{-0x1.fff336p-1f,-0x1.fff336p-1f}, 0x1.020fdcp-1f}, //     0  
  {{-0x1.fff26cp-1f,-0x1.fff26cp-1f}, 0x1.021fe0p-1f}, //     0  
  {{-0x1.fff19cp-1f,-0x1.fff19cp-1f}, 0x1.022fe4p-1f}, //     0  
  {{-0x1.fff0c6p-1f,-0x1.fff0c6p-1f}, 0x1.023fe6p-1f}, //     0  
  {{-0x1.ffefeap-1f,-0x1.ffefeap-1f}, 0x1.024fe6p-1f}, //     0  
  {{-0x1.ffef08p-1f,-0x1.ffef08p-1f}, 0x1.025fe8p-1f}, //     0  
  {{-0x1.ffee20p-1f,-0x1.ffee20p-1f}, 0x1.026fe6p-1f}, //     0  
  {{-0x1.ffed32p-1f,-0x1.ffed32p-1f}, 0x1.027fe4p-1f}, //     0  
  {{-0x1.ffec3ep-1f,-0x1.ffec3ep-1f}, 0x1.028fe2p-1f}, //     0  
  {{-0x1.ffeb44p-1f,-0x1.ffeb44p-1f}, 0x1.029fdep-1f}, //     0  
  {{-0x1.ffea44p-1f,-0x1.ffea44p-1f}, 0x1.02afdap-1f}, //     0  
  {{-0x1.ffe93ep-1f,-0x1.ffe93ep-1f}, 0x1.02bfd4p-1f}, //     0  
  {{-0x1.ffe832p-1f,-0x1.ffe832p-1f}, 0x1.02cfcep-1f}, //     0  
  {{-0x1.ffe720p-1f,-0x1.ffe720p-1f}, 0x1.02dfc8p-1f}, //     0  
  {{-0x1.ffe606p-1f,-0x1.ffe606p-1f}, 0x1.02efdep-1f}, //     0  
  {{-0x1.ffe4eap-1f,-0x1.ffe4eap-1f}, 0x1.02ffbap-1f}, //     0  
  {{-0x1.ffe3c4p-1f,-0x1.ffe3c4p-1f}, 0x1.030fccp-1f}, //     0  
  {{-0x1.ffe29ap-1f,-0x1.ffe29ap-1f}, 0x1.031fc2p-1f}, //     0  
  {{-0x1.ffe168p-1f,-0x1.ffe168p-1f}, 0x1.032fd2p-1f}, //     0  
  {{-0x1.ffe032p-1f,-0x1.ffe032p-1f}, 0x1.033fc8p-1f}, //     0  
  {{-0x1.ffdef4p-1f,-0x1.ffdef4p-1f}, 0x1.034fd6p-1f}, //     0  
  {{-0x1.ffddb2p-1f,-0x1.ffddb2p-1f}, 0x1.035fc8p-1f}, //     0  
  {{-0x1.ffdc68p-1f,-0x1.ffdc68p-1f}, 0x1.036fd4p-1f}, //     0  
  {{-0x1.ffdb1ap-1f,-0x1.ffdb1ap-1f}, 0x1.037fc4p-1f}, //     0  
  {{-0x1.ffd9c4p-1f,-0x1.ffd9c4p-1f}, 0x1.038fcep-1f}, //     0  
  {{-0x1.ffd86ap-1f,-0x1.ffd86ap-1f}, 0x1.039fbep-1f}, //     0  
  {{-0x1.ffd708p-1f,-0x1.ffd708p-1f}, 0x1.03afc4p-1f}, //     0  
  {{-0x1.ffd5a0p-1f,-0x1.ffd5a0p-1f}, 0x1.03bfcap-1f}, //     0  
  {{-0x1.ffd432p-1f,-0x1.ffd432p-1f}, 0x1.03cfcep-1f}, //     0  
  {{-0x1.ffd2c0p-1f,-0x1.ffd2c0p-1f}, 0x1.03dfbcp-1f}, //     0  
  {{-0x1.ffd144p-1f,-0x1.ffd144p-1f}, 0x1.03efd4p-1f}, //     0  
  {{-0x1.ffcfc6p-1f,-0x1.ffcfc6p-1f}, 0x1.03ffc0p-1f}, //     0  
  {{-0x1.ffce42p-1f,-0x1.ffce42p-1f}, 0x1.040facp-1f}, //     0  
  {{-0x1.ffccb4p-1f,-0x1.ffccb4p-1f}, 0x1.041fc0p-1f}, //     0  
  {{-0x1.ffcb22p-1f,-0x1.ffcb22p-1f}, 0x1.042fc0p-1f}, //     0  
  {{-0x1.ffc98ap-1f,-0x1.ffc98ap-1f}, 0x1.043fbcp-1f}, //     0  
  {{-0x1.ffc7ecp-1f,-0x1.ffc7ecp-1f}, 0x1.044fbap-1f}, //     0  
  {{-0x1.ffc648p-1f,-0x1.ffc648p-1f}, 0x1.045fb6p-1f}, //     0  
  {{-0x1.ffc49ep-1f,-0x1.ffc49ep-1f}, 0x1.046fb0p-1f}, //     0  
  {{-0x1.ffc2ecp-1f,-0x1.ffc2ecp-1f}, 0x1.047fbep-1f}, //     0  
  {{-0x1.ffc136p-1f,-0x1.ffc136p-1f}, 0x1.048fb6p-1f}, //     0  
  {{-0x1.ffbf78p-1f,-0x1.ffbf78p-1f}, 0x1.049fc0p-1f}, //     0  
  {{-0x1.ffbdb6p-1f,-0x1.ffbdb6p-1f}, 0x1.04afb8p-1f}, //     0  
  {{-0x1.ffbbecp-1f,-0x1.ffbbecp-1f}, 0x1.04bfc0p-1f}, //     0  
  {{-0x1.ffba1ep-1f,-0x1.ffba1ep-1f}, 0x1.04cfb6p-1f}, //     0  
  {{-0x1.ffb848p-1f,-0x1.ffb848p-1f}, 0x1.04dfbcp-1f}, //     0  
  {{-0x1.ffb66ep-1f,-0x1.ffb66ep-1f}, 0x1.04efb0p-1f}, //     0  
  {{-0x1.ffb48ep-1f,-0x1.ffb48ep-1f}, 0x1.04ffa4p-1f}, //     0  
  {{-0x1.ffb2a4p-1f,-0x1.ffb2a4p-1f}, 0x1.050fb6p-1f}, //     0  
  {{-0x1.ffb0b8p-1f,-0x1.ffb0b8p-1f}, 0x1.051fa8p-1f}, //     0  
  {{-0x1.ffaec4p-1f,-0x1.ffaec4p-1f}, 0x1.052faap-1f}, //     0  
  {{-0x1.ffaccap-1f,-0x1.ffaccap-1f}, 0x1.053faap-1f}, //     0  
  {{-0x1.ffaacap-1f,-0x1.ffaacap-1f}, 0x1.054faap-1f}, //     0  
  {{-0x1.ffa8c4p-1f,-0x1.ffa8c4p-1f}, 0x1.055fa8p-1f}, //     0  
  {{-0x1.ffa6b6p-1f,-0x1.ffa6b6p-1f}, 0x1.056fb4p-1f}, //     0  
  {{-0x1.ffa4a6p-1f,-0x1.ffa4a6p-1f}, 0x1.057fa2p-1f}, //     0  
  {{-0x1.ffa28cp-1f,-0x1.ffa28cp-1f}, 0x1.058facp-1f}, //     0  
  {{-0x1.ffa06ep-1f,-0x1.ffa06ep-1f}, 0x1.059fa6p-1f}, //     0  
  {{-0x1.ff9e48p-1f,-0x1.ff9e48p-1f}, 0x1.05afaep-1f}, //     0  
  {{-0x1.ff9c1ep-1f,-0x1.ff9c1ep-1f}, 0x1.05bfa8p-1f}, //     0  
  {{-0x1.ff99eep-1f,-0x1.ff99eep-1f}, 0x1.05cf9ep-1f}, //     0  
  {{-0x1.ff97b6p-1f,-0x1.ff97b6p-1f}, 0x1.05dfa4p-1f}, //     0  
  {{-0x1.ff957ap-1f,-0x1.ff957ap-1f}, 0x1.05ef9ap-1f}, //     0  
  {{-0x1.ff9336p-1f,-0x1.ff9336p-1f}, 0x1.05ff9ep-1f}, //     0  
  {{-0x1.ff90ecp-1f,-0x1.ff90ecp-1f}, 0x1.060fa0p-1f}, //     0  
  {{-0x1.ff8e9cp-1f,-0x1.ff8e9cp-1f}, 0x1.061fa2p-1f}, //     0  
  {{-0x1.ff8c46p-1f,-0x1.ff8c46p-1f}, 0x1.062fa2p-1f}, //     0  
  {{-0x1.ff89eap-1f,-0x1.ff89eap-1f}, 0x1.063fa0p-1f}, //     0  
  {{-0x1.ff8788p-1f,-0x1.ff8788p-1f}, 0x1.064f9ep-1f}, //     0  
  {{-0x1.ff8522p-1f,-0x1.ff8522p-1f}, 0x1.065f90p-1f}, //     0  
  {{-0x1.ff82b2p-1f,-0x1.ff82b2p-1f}, 0x1.066f98p-1f}, //     0  
  {{-0x1.ff803ep-1f,-0x1.ff803ep-1f}, 0x1.067f94p-1f}, //     0  
  {{-0x1.ff7dc2p-1f,-0x1.ff7dc2p-1f}, 0x1.068f9cp-1f}, //     0  
  {{-0x1.ff7b42p-1f,-0x1.ff7b42p-1f}, 0x1.069f96p-1f}, //     0  
  {{-0x1.ff78bap-1f,-0x1.ff78bap-1f}, 0x1.06af9ap-1f}, //     0  
  {{-0x1.ff762ep-1f,-0x1.ff762ep-1f}, 0x1.06bf92p-1f}, //     0  
  {{-0x1.ff739ap-1f,-0x1.ff739ap-1f}, 0x1.06cf96p-1f}, //     0  
  {{-0x1.ff7102p-1f,-0x1.ff7102p-1f}, 0x1.06df8cp-1f}, //     0  
  {{-0x1.ff6e60p-1f,-0x1.ff6e60p-1f}, 0x1.06ef9ap-1f}, //     0  
  {{-0x1.ff6bbcp-1f,-0x1.ff6bbcp-1f}, 0x1.06ff8ep-1f}, //     0  
  {{-0x1.ff6910p-1f,-0x1.ff6910p-1f}, 0x1.070f8ep-1f}, //     0  
  {{-0x1.ff665ep-1f,-0x1.ff665ep-1f}, 0x1.071f8cp-1f}, //     0  
  {{-0x1.ff63a6p-1f,-0x1.ff63a6p-1f}, 0x1.072f8ap-1f}, //     0  
  {{-0x1.ff60e6p-1f,-0x1.ff60e6p-1f}, 0x1.073f92p-1f}, //     0  
  {{-0x1.ff5e22p-1f,-0x1.ff5e22p-1f}, 0x1.074f8cp-1f}, //     0  
  {{-0x1.ff5b56p-1f,-0x1.ff5b56p-1f}, 0x1.075f92p-1f}, //     0  
  {{-0x1.ff5886p-1f,-0x1.ff5886p-1f}, 0x1.076f8cp-1f}, //     0  
  {{-0x1.ff55b0p-1f,-0x1.ff55b0p-1f}, 0x1.077f86p-1f}, //     0  
  {{-0x1.ff52d2p-1f,-0x1.ff52d2p-1f}, 0x1.078f88p-1f}, //     0  
  {{-0x1.ff4ff0p-1f,-0x1.ff4ff0p-1f}, 0x1.079f80p-1f}, //     0  
  {{-0x1.ff4d04p-1f,-0x1.ff4d04p-1f}, 0x1.07af8cp-1f}, //     0  
  {{-0x1.ff4a16p-1f,-0x1.ff4a16p-1f}, 0x1.07bf80p-1f}, //     0  
  {{-0x1.ff471ep-1f,-0x1.ff471ep-1f}, 0x1.07cf8ap-1f}, //     0  
  {{-0x1.ff4424p-1f,-0x1.ff4424p-1f}, 0x1.07df7ep-1f}, //     0  
  {{-0x1.ff4120p-1f,-0x1.ff4120p-1f}, 0x1.07ef86p-1f}, //     0  
  {{-0x1.ff3e18p-1f,-0x1.ff3e18p-1f}, 0x1.07ff82p-1f}, //     0  
  {{-0x1.ff3b0ap-1f,-0x1.ff3b0ap-1f}, 0x1.080f7ep-1f}, //     0  
  {{-0x1.ff37f4p-1f,-0x1.ff37f4p-1f}, 0x1.081f82p-1f}, //     0  
  {{-0x1.ff34dap-1f,-0x1.ff34dap-1f}, 0x1.082f7cp-1f}, //     0  
  {{-0x1.ff31b8p-1f,-0x1.ff31b8p-1f}, 0x1.083f7ep-1f}, //     0  
  {{-0x1.ff2e92p-1f,-0x1.ff2e92p-1f}, 0x1.084f76p-1f}, //     0  
  {{-0x1.ff2b62p-1f,-0x1.ff2b62p-1f}, 0x1.085f80p-1f}, //     0  
  {{-0x1.ff2830p-1f,-0x1.ff2830p-1f}, 0x1.086f76p-1f}, //     0  
  {{-0x1.ff24f6p-1f,-0x1.ff24f6p-1f}, 0x1.087f76p-1f}, //     0  
  {{-0x1.ff21b4p-1f,-0x1.ff21b4p-1f}, 0x1.088f7cp-1f}, //     0  
  {{-0x1.ff1e70p-1f,-0x1.ff1e70p-1f}, 0x1.089f70p-1f}, //     0  
  {{-0x1.ff1b22p-1f,-0x1.ff1b22p-1f}, 0x1.08af76p-1f}, //     0  
  {{-0x1.ff17d0p-1f,-0x1.ff17d0p-1f}, 0x1.08bf70p-1f}, //     0  
  {{-0x1.ff1476p-1f,-0x1.ff1476p-1f}, 0x1.08cf74p-1f}, //     0  
  {{-0x1.ff1118p-1f,-0x1.ff1118p-1f}, 0x1.08df6ep-1f}, //     0  
  {{-0x1.ff0db0p-1f,-0x1.ff0db0p-1f}, 0x1.08ef78p-1f}, //     0  
  {{-0x1.ff0a46p-1f,-0x1.ff0a46p-1f}, 0x1.08ff70p-1f}, //     0  
  {{-0x1.ff06d4p-1f,-0x1.ff06d4p-1f}, 0x1.090f70p-1f}, //     0  
  {{-0x1.ff035cp-1f,-0x1.ff035cp-1f}, 0x1.091f6ep-1f}, //     0  
  {{-0x1.feffdep-1f,-0x1.feffdep-1f}, 0x1.092f6cp-1f}, //     0  
  {{-0x1.fefc58p-1f,-0x1.fefc58p-1f}, 0x1.093f72p-1f}, //     0  
  {{-0x1.fef8cep-1f,-0x1.fef8cep-1f}, 0x1.094f6ep-1f}, //     0  
  {{-0x1.fef53cp-1f,-0x1.fef53cp-1f}, 0x1.095f72p-1f}, //     0  
  {{-0x1.fef1a6p-1f,-0x1.fef1a6p-1f}, 0x1.096f6ap-1f}, //     0  
  {{-0x1.feee0ap-1f,-0x1.feee0ap-1f}, 0x1.097f64p-1f}, //     0  
  {{-0x1.feea64p-1f,-0x1.feea64p-1f}, 0x1.098f6ep-1f}, //     0  
  {{-0x1.fee6bcp-1f,-0x1.fee6bcp-1f}, 0x1.099f64p-1f}, //     0  
  {{-0x1.fee30ap-1f,-0x1.fee30ap-1f}, 0x1.09af6cp-1f}, //     0  
  {{-0x1.fedf56p-1f,-0x1.fedf56p-1f}, 0x1.09bf62p-1f}, //     0  
  {{-0x1.fedb98p-1f,-0x1.fedb98p-1f}, 0x1.09cf68p-1f}, //     0  
  {{-0x1.fed7d6p-1f,-0x1.fed7d6p-1f}, 0x1.09df64p-1f}, //     0  
  {{-0x1.fed40ep-1f,-0x1.fed40ep-1f}, 0x1.09ef5ep-1f}, //     0  
  {{-0x1.fed03ep-1f,-0x1.fed03ep-1f}, 0x1.09ff60p-1f}, //     0  
  {{-0x1.fecc6ap-1f,-0x1.fecc6ap-1f}, 0x1.0a0f5ap-1f}, //     0  
  {{-0x1.fec88cp-1f,-0x1.fec88cp-1f}, 0x1.0a1f64p-1f}, //     0  
  {{-0x1.fec4acp-1f,-0x1.fec4acp-1f}, 0x1.0a2f5ap-1f}, //     0  
  {{-0x1.fec0c4p-1f,-0x1.fec0c4p-1f}, 0x1.0a3f5ap-1f}, //     0  
  {{-0x1.febcd4p-1f,-0x1.febcd4p-1f}, 0x1.0a4f5ep-1f}, //     0  
  {{-0x1.feb8e0p-1f,-0x1.feb8e0p-1f}, 0x1.0a5f5cp-1f}, //     0  
  {{-0x1.feb4e4p-1f,-0x1.feb4e4p-1f}, 0x1.0a6f60p-1f}, //     0  
  {{-0x1.feb0e4p-1f,-0x1.feb0e4p-1f}, 0x1.0a7f5ap-1f}, //     0  
  {{-0x1.feacdcp-1f,-0x1.feacdcp-1f}, 0x1.0a8f5cp-1f}, //     0  
  {{-0x1.fea8d0p-1f,-0x1.fea8d0p-1f}, 0x1.0a9f54p-1f}, //     0  
  {{-0x1.fea4bcp-1f,-0x1.fea4bcp-1f}, 0x1.0aaf54p-1f}, //     0  
  {{-0x1.fea0a2p-1f,-0x1.fea0a2p-1f}, 0x1.0abf54p-1f}, //     0  
  {{-0x1.fe9c82p-1f,-0x1.fe9c82p-1f}, 0x1.0acf52p-1f}, //     0  
  {{-0x1.fe985ap-1f,-0x1.fe985ap-1f}, 0x1.0adf56p-1f}, //     0  
  {{-0x1.fe942ep-1f,-0x1.fe942ep-1f}, 0x1.0aef52p-1f}, //     0  
  {{-0x1.fe8ffcp-1f,-0x1.fe8ffcp-1f}, 0x1.0aff4ep-1f}, //     0  
  {{-0x1.fe8bc2p-1f,-0x1.fe8bc2p-1f}, 0x1.0b0f50p-1f}, //     0  
  {{-0x1.fe8784p-1f,-0x1.fe8784p-1f}, 0x1.0b1f48p-1f}, //     0  
  {{-0x1.fe833cp-1f,-0x1.fe833cp-1f}, 0x1.0b2f50p-1f}, //     0  
  {{-0x1.fe7ef0p-1f,-0x1.fe7ef0p-1f}, 0x1.0b3f4ep-1f}, //     0  
  {{-0x1.fe7a9ep-1f,-0x1.fe7a9ep-1f}, 0x1.0b4f4cp-1f}, //     0  
  {{-0x1.fe7646p-1f,-0x1.fe7646p-1f}, 0x1.0b5f4ap-1f}, //     0  
  {{-0x1.fe71e8p-1f,-0x1.fe71e8p-1f}, 0x1.0b6f46p-1f}, //     0  
  {{-0x1.fe6d82p-1f,-0x1.fe6d82p-1f}, 0x1.0b7f48p-1f}, //     0  
  {{-0x1.fe6916p-1f,-0x1.fe6916p-1f}, 0x1.0b8f48p-1f}, //     0  
  {{-0x1.fe64a4p-1f,-0x1.fe64a4p-1f}, 0x1.0b9f48p-1f}, //     0  
  {{-0x1.fe602cp-1f,-0x1.fe602cp-1f}, 0x1.0baf48p-1f}, //     0  
  {{-0x1.fe5bb0p-1f,-0x1.fe5bb0p-1f}, 0x1.0bbf40p-1f}, //     0  
  {{-0x1.fe572ap-1f,-0x1.fe572ap-1f}, 0x1.0bcf44p-1f}, //     0  
  {{-0x1.fe52a0p-1f,-0x1.fe52a0p-1f}, 0x1.0bdf40p-1f}, //     0  
  {{-0x1.fe4e0ep-1f,-0x1.fe4e0ep-1f}, 0x1.0bef42p-1f}, //     0  
  {{-0x1.fe4978p-1f,-0x1.fe4978p-1f}, 0x1.0bff3ep-1f}, //     0  
  {{-0x1.fe44dap-1f,-0x1.fe44dap-1f}, 0x1.0c0f3ep-1f}, //     0  
  {{-0x1.fe4036p-1f,-0x1.fe4036p-1f}, 0x1.0c1f3ep-1f}, //     0  
  {{-0x1.fe3b8cp-1f,-0x1.fe3b8cp-1f}, 0x1.0c2f3cp-1f}, //     0  
  {{-0x1.fe36dap-1f,-0x1.fe36dap-1f}, 0x1.0c3f40p-1f}, //     0  
  {{-0x1.fe3224p-1f,-0x1.fe3224p-1f}, 0x1.0c4f3ep-1f}, //     0  
  {{-0x1.fe2d68p-1f,-0x1.fe2d68p-1f}, 0x1.0c5f38p-1f}, //     0  
  {{-0x1.fe28a4p-1f,-0x1.fe28a4p-1f}, 0x1.0c6f3ap-1f}, //     0  
  {{-0x1.fe23dcp-1f,-0x1.fe23dcp-1f}, 0x1.0c7f34p-1f}, //     0  
  {{-0x1.fe1f0ap-1f,-0x1.fe1f0ap-1f}, 0x1.0c8f3ap-1f}, //     0  
  {{-0x1.fe1a36p-1f,-0x1.fe1a36p-1f}, 0x1.0c9f32p-1f}, //     0  
  {{-0x1.fe155ap-1f,-0x1.fe155ap-1f}, 0x1.0caf30p-1f}, //     0  
  {{-0x1.fe1076p-1f,-0x1.fe1076p-1f}, 0x1.0cbf34p-1f}, //     0  
  {{-0x1.fe0b8ep-1f,-0x1.fe0b8ep-1f}, 0x1.0ccf30p-1f}, //     0  
  {{-0x1.fe069cp-1f,-0x1.fe069cp-1f}, 0x1.0cdf36p-1f}, //     0  
  {{-0x1.fe01a8p-1f,-0x1.fe01a8p-1f}, 0x1.0cef30p-1f}, //     0  
  {{-0x1.fdfcacp-1f,-0x1.fdfcacp-1f}, 0x1.0cff30p-1f}, //     0  
  {{-0x1.fdf7aap-1f,-0x1.fdf7aap-1f}, 0x1.0d0f2ep-1f}, //     0  
  {{-0x1.fdf2a2p-1f,-0x1.fdf2a2p-1f}, 0x1.0d1f2cp-1f}, //     0  
  {{-0x1.fded92p-1f,-0x1.fded92p-1f}, 0x1.0d2f2ep-1f}, //     0  
  {{-0x1.fde87ep-1f,-0x1.fde87ep-1f}, 0x1.0d3f2ap-1f}, //     0  
  {{-0x1.fde362p-1f,-0x1.fde362p-1f}, 0x1.0d4f2ap-1f}, //     0  
  {{-0x1.fdde40p-1f,-0x1.fdde40p-1f}, 0x1.0d5f2ap-1f}, //     0  
  {{-0x1.fdd918p-1f,-0x1.fdd918p-1f}, 0x1.0d6f2ap-1f}, //     0  
  {{-0x1.fdd3e8p-1f,-0x1.fdd3e8p-1f}, 0x1.0d7f2ep-1f}, //     0  
  {{-0x1.fdceb4p-1f,-0x1.fdceb4p-1f}, 0x1.0d8f2ap-1f}, //     0  
  {{-0x1.fdc97ap-1f,-0x1.fdc97ap-1f}, 0x1.0d9f26p-1f}, //     0  
  {{-0x1.fdc438p-1f,-0x1.fdc438p-1f}, 0x1.0daf28p-1f}, //     0  
  {{-0x1.fdbef2p-1f,-0x1.fdbef2p-1f}, 0x1.0dbf22p-1f}, //     0  
  {{-0x1.fdb9a2p-1f,-0x1.fdb9a2p-1f}, 0x1.0dcf26p-1f}, //     0  
  {{-0x1.fdb44ep-1f,-0x1.fdb44ep-1f}, 0x1.0ddf24p-1f}, //     0  
  {{-0x1.fdaef4p-1f,-0x1.fdaef4p-1f}, 0x1.0def22p-1f}, //     0  
  {{-0x1.fda992p-1f,-0x1.fda992p-1f}, 0x1.0dff24p-1f}, //     0  
  {{-0x1.fda42cp-1f,-0x1.fda42cp-1f}, 0x1.0e0f20p-1f}, //     0  
  {{-0x1.fd9ebep-1f,-0x1.fd9ebep-1f}, 0x1.0e1f20p-1f}, //     0  
  {{-0x1.fd994ap-1f,-0x1.fd994ap-1f}, 0x1.0e2f20p-1f}, //     0  
  {{-0x1.fd93d2p-1f,-0x1.fd93d2p-1f}, 0x1.0e3f18p-1f}, //     0  
  {{-0x1.fd8e50p-1f,-0x1.fd8e50p-1f}, 0x1.0e4f1cp-1f}, //     0  
  {{-0x1.fd88cap-1f,-0x1.fd88cap-1f}, 0x1.0e5f18p-1f}, //     0  
  {{-0x1.fd833ap-1f,-0x1.fd833ap-1f}, 0x1.0e6f1ep-1f}, //     0  
  {{-0x1.fd7da8p-1f,-0x1.fd7da8p-1f}, 0x1.0e7f1ap-1f}, //     0  
  {{-0x1.fd780ep-1f,-0x1.fd780ep-1f}, 0x1.0e8f18p-1f}, //     0  
  {{-0x1.fd726ep-1f,-0x1.fd726ep-1f}, 0x1.0e9f16p-1f}, //     0  
  {{-0x1.fd6cc8p-1f,-0x1.fd6cc8p-1f}, 0x1.0eaf14p-1f}, //     0  
  {{-0x1.fd671cp-1f,-0x1.fd671cp-1f}, 0x1.0ebf10p-1f}, //     0  
  {{-0x1.fd6166p-1f,-0x1.fd6166p-1f}, 0x1.0ecf18p-1f}, //     0  
  {{-0x1.fd5baep-1f,-0x1.fd5baep-1f}, 0x1.0edf12p-1f}, //     0  
  {{-0x1.fd55ecp-1f,-0x1.fd55ecp-1f}, 0x1.0eef16p-1f}, //     0  
  {{-0x1.fd5028p-1f,-0x1.fd5028p-1f}, 0x1.0eff10p-1f}, //     0  
  {{-0x1.fd4a5cp-1f,-0x1.fd4a5cp-1f}, 0x1.0f0f0cp-1f}, //     0  
  {{-0x1.fd4488p-1f,-0x1.fd4488p-1f}, 0x1.0f1f0ep-1f}, //     0  
  {{-0x1.fd3eaep-1f,-0x1.fd3eaep-1f}, 0x1.0f2f10p-1f}, //     0  
  {{-0x1.fd38d0p-1f,-0x1.fd38d0p-1f}, 0x1.0f3f0ap-1f}, //     0  
  {{-0x1.fd32e8p-1f,-0x1.fd32e8p-1f}, 0x1.0f4f0ep-1f}, //     0  
  {{-0x1.fd2cfep-1f,-0x1.fd2cfep-1f}, 0x1.0f5f06p-1f}, //     0  
  {{-0x1.fd2708p-1f,-0x1.fd2708p-1f}, 0x1.0f6f0ep-1f}, //     0  
  {{-0x1.fd2110p-1f,-0x1.fd2110p-1f}, 0x1.0f7f0ap-1f}, //     0  
  {{-0x1.fd1b12p-1f,-0x1.fd1b12p-1f}, 0x1.0f8f06p-1f}, //     0  
  {{-0x1.fd150cp-1f,-0x1.fd150cp-1f}, 0x1.0f9f06p-1f}, //     0  
  {{-0x1.fd0f00p-1f,-0x1.fd0f00p-1f}, 0x1.0faf04p-1f}, //     0  
  {{-0x1.fd08eep-1f,-0x1.fd08eep-1f}, 0x1.0fbf02p-1f}, //     0  
  {{-0x1.fd02d4p-1f,-0x1.fd02d4p-1f}, 0x1.0fcf02p-1f}, //     0  
  {{-0x1.fcfcb6p-1f,-0x1.fcfcb6p-1f}, 0x1.0fdefep-1f}, //     0  
  {{-0x1.fcf68ep-1f,-0x1.fcf68ep-1f}, 0x1.0fef04p-1f}, //     0  
  {{-0x1.fcf062p-1f,-0x1.fcf062p-1f}, 0x1.0fff02p-1f}, //     0  
  {{-0x1.fcea32p-1f,-0x1.fcea32p-1f}, 0x1.100efcp-1f}, //     0  
  {{-0x1.fce3f8p-1f,-0x1.fce3f8p-1f}, 0x1.101efep-1f}, //     0  
  {{-0x1.fcddb8p-1f,-0x1.fcddb8p-1f}, 0x1.102efep-1f}, //     0  
  {{-0x1.fcd774p-1f,-0x1.fcd774p-1f}, 0x1.103efap-1f}, //     0  
  {{-0x1.fcd126p-1f,-0x1.fcd126p-1f}, 0x1.104efep-1f}, //     0  
  {{-0x1.fccad6p-1f,-0x1.fccad6p-1f}, 0x1.105ef8p-1f}, //     0  
  {{-0x1.fcc47cp-1f,-0x1.fcc47cp-1f}, 0x1.106efap-1f}, //     0  
  {{-0x1.fcbe1ep-1f,-0x1.fcbe1ep-1f}, 0x1.107ef6p-1f}, //     0  
  {{-0x1.fcb7b8p-1f,-0x1.fcb7b8p-1f}, 0x1.108ef6p-1f}, //     0  
  {{-0x1.fcb14cp-1f,-0x1.fcb14cp-1f}, 0x1.109ef6p-1f}, //     0  
  {{-0x1.fcaadap-1f,-0x1.fcaadap-1f}, 0x1.10aef4p-1f}, //     0  
  {{-0x1.fca462p-1f,-0x1.fca462p-1f}, 0x1.10bef2p-1f}, //     0  
  {{-0x1.fc9de2p-1f,-0x1.fc9de2p-1f}, 0x1.10cef4p-1f}, //     0  
  {{-0x1.fc975ep-1f,-0x1.fc975ep-1f}, 0x1.10def0p-1f}, //     0  
  {{-0x1.fc90d0p-1f,-0x1.fc90d0p-1f}, 0x1.10eef4p-1f}, //     0  
  {{-0x1.fc8a3ep-1f,-0x1.fc8a3ep-1f}, 0x1.10fef2p-1f}, //     0  
  {{-0x1.fc83a6p-1f,-0x1.fc83a6p-1f}, 0x1.110ef0p-1f}, //     0  
  {{-0x1.fc7d06p-1f,-0x1.fc7d06p-1f}, 0x1.111ef2p-1f}, //     0  
  {{-0x1.fc7662p-1f,-0x1.fc7662p-1f}, 0x1.112eeep-1f}, //     0  
  {{-0x1.fc6fb8p-1f,-0x1.fc6fb8p-1f}, 0x1.113ee8p-1f}, //     0  
  {{-0x1.fc6904p-1f,-0x1.fc6904p-1f}, 0x1.114eecp-1f}, //     0  
  {{-0x1.fc624cp-1f,-0x1.fc624cp-1f}, 0x1.115eeap-1f}, //     0  
  {{-0x1.fc5b8cp-1f,-0x1.fc5b8cp-1f}, 0x1.116eeap-1f}, //     0  
  {{-0x1.fc54c8p-1f,-0x1.fc54c8p-1f}, 0x1.117ee6p-1f}, //     0  
  {{-0x1.fc4dfcp-1f,-0x1.fc4dfcp-1f}, 0x1.118ee6p-1f}, //     0  
  {{-0x1.fc4728p-1f,-0x1.fc4728p-1f}, 0x1.119eeap-1f}, //     0  
  {{-0x1.fc4050p-1f,-0x1.fc4050p-1f}, 0x1.11aee8p-1f}, //     0  
  {{-0x1.fc3972p-1f,-0x1.fc3972p-1f}, 0x1.11bee4p-1f}, //     0  
  {{-0x1.fc328cp-1f,-0x1.fc328cp-1f}, 0x1.11cee4p-1f}, //     0  
  {{-0x1.fc2ba0p-1f,-0x1.fc2ba0p-1f}, 0x1.11dee4p-1f}, //     0  
  {{-0x1.fc24aep-1f,-0x1.fc24aep-1f}, 0x1.11eee2p-1f}, //     0  
  {{-0x1.fc1db6p-1f,-0x1.fc1db6p-1f}, 0x1.11fee0p-1f}, //     0  
  {{-0x1.fc16b6p-1f,-0x1.fc16b6p-1f}, 0x1.120ee0p-1f}, //     0  
  {{-0x1.fc0fb2p-1f,-0x1.fc0fb2p-1f}, 0x1.121edcp-1f}, //     0  
  {{-0x1.fc08a4p-1f,-0x1.fc08a4p-1f}, 0x1.122ee0p-1f}, //     0  
  {{-0x1.fc0194p-1f,-0x1.fc0194p-1f}, 0x1.123edap-1f}, //     0  
  {{-0x1.fbfa78p-1f,-0x1.fbfa78p-1f}, 0x1.124ee0p-1f}, //     0  
  {{-0x1.fbf35ap-1f,-0x1.fbf35ap-1f}, 0x1.125edcp-1f}, //     0  
  {{-0x1.fbec36p-1f,-0x1.fbec36p-1f}, 0x1.126ed8p-1f}, //     0  
  {{-0x1.fbe508p-1f,-0x1.fbe508p-1f}, 0x1.127edap-1f}, //     0  
  {{-0x1.fbddd6p-1f,-0x1.fbddd6p-1f}, 0x1.128ed8p-1f}, //     0  
  {{-0x1.fbd69ep-1f,-0x1.fbd69ep-1f}, 0x1.129ed6p-1f}, //     0  
  {{-0x1.fbcf5ep-1f,-0x1.fbcf5ep-1f}, 0x1.12aed6p-1f}, //     0  
  {{-0x1.fbc818p-1f,-0x1.fbc818p-1f}, 0x1.12bed6p-1f}, //     0  
  {{-0x1.fbc0cep-1f,-0x1.fbc0cep-1f}, 0x1.12ced0p-1f}, //     0  
  {{-0x1.fbb97ap-1f,-0x1.fbb97ap-1f}, 0x1.12ded2p-1f}, //     0  
  {{-0x1.fbb220p-1f,-0x1.fbb220p-1f}, 0x1.12eed2p-1f}, //     0  
  {{-0x1.fbaac2p-1f,-0x1.fbaac2p-1f}, 0x1.12fecep-1f}, //     0  
  {{-0x1.fba35ap-1f,-0x1.fba35ap-1f}, 0x1.130ed2p-1f}, //     0  
  {{-0x1.fb9bf0p-1f,-0x1.fb9bf0p-1f}, 0x1.131eccp-1f}, //     0  
  {{-0x1.fb947ap-1f,-0x1.fb947ap-1f}, 0x1.132ed2p-1f}, //     0  
  {{-0x1.fb8d02p-1f,-0x1.fb8d02p-1f}, 0x1.133ecep-1f}, //     0  
  {{-0x1.fb8584p-1f,-0x1.fb8584p-1f}, 0x1.134ecap-1f}, //     0  
  {{-0x1.fb7dfcp-1f,-0x1.fb7dfcp-1f}, 0x1.135eccp-1f}, //     0  
  {{-0x1.fb7670p-1f,-0x1.fb7670p-1f}, 0x1.136ecap-1f}, //     0  
  {{-0x1.fb6edep-1f,-0x1.fb6edep-1f}, 0x1.137ec6p-1f}, //     0  
  {{-0x1.fb6742p-1f,-0x1.fb6742p-1f}, 0x1.138ecap-1f}, //     0  
  {{-0x1.fb5fa4p-1f,-0x1.fb5fa4p-1f}, 0x1.139ec6p-1f}, //     0  
  {{-0x1.fb57fep-1f,-0x1.fb57fep-1f}, 0x1.13aec4p-1f}, //     0  
  {{-0x1.fb5050p-1f,-0x1.fb5050p-1f}, 0x1.13bec4p-1f}, //     0  
  {{-0x1.fb489cp-1f,-0x1.fb489cp-1f}, 0x1.13cec4p-1f}, //     0  
  {{-0x1.fb40e4p-1f,-0x1.fb40e4p-1f}, 0x1.13dec0p-1f}, //     0  
  {{-0x1.fb3922p-1f,-0x1.fb3922p-1f}, 0x1.13eec2p-1f}, //     0  
  {{-0x1.fb315cp-1f,-0x1.fb315cp-1f}, 0x1.13fec0p-1f}, //     0  
  {{-0x1.fb2990p-1f,-0x1.fb2990p-1f}, 0x1.140ebcp-1f}, //     0  
  {{-0x1.fb21bap-1f,-0x1.fb21bap-1f}, 0x1.141ec0p-1f}, //     0  
  {{-0x1.fb19e0p-1f,-0x1.fb19e0p-1f}, 0x1.142ebep-1f}, //     0  
  {{-0x1.fb1200p-1f,-0x1.fb1200p-1f}, 0x1.143ebcp-1f}, //     0  
  {{-0x1.fb0a18p-1f,-0x1.fb0a18p-1f}, 0x1.144ebcp-1f}, //     0  
  {{-0x1.fb022cp-1f,-0x1.fb022cp-1f}, 0x1.145eb8p-1f}, //     0  
  {{-0x1.fafa38p-1f,-0x1.fafa38p-1f}, 0x1.146eb8p-1f}, //     0  
  {{-0x1.faf23cp-1f,-0x1.faf23cp-1f}, 0x1.147ebap-1f}, //     0  
  {{-0x1.faea3cp-1f,-0x1.faea3cp-1f}, 0x1.148eb6p-1f}, //     0  
  {{-0x1.fae236p-1f,-0x1.fae236p-1f}, 0x1.149eb2p-1f}, //     0  
  {{-0x1.fada26p-1f,-0x1.fada26p-1f}, 0x1.14aeb6p-1f}, //     0  
  {{-0x1.fad212p-1f,-0x1.fad212p-1f}, 0x1.14beb4p-1f}, //     0  
  {{-0x1.fac9f6p-1f,-0x1.fac9f6p-1f}, 0x1.14ceb4p-1f}, //     0  
  {{-0x1.fac1d4p-1f,-0x1.fac1d4p-1f}, 0x1.14deb4p-1f}, //     0  
  {{-0x1.fab9aep-1f,-0x1.fab9aep-1f}, 0x1.14eeb0p-1f}, //     0  
  {{-0x1.fab180p-1f,-0x1.fab180p-1f}, 0x1.14feaep-1f}, //     0  
  {{-0x1.faa94ap-1f,-0x1.faa94ap-1f}, 0x1.150eb0p-1f}, //     0  
  {{-0x1.faa110p-1f,-0x1.faa110p-1f}, 0x1.151eacp-1f}, //     0  
  {{-0x1.fa98ccp-1f,-0x1.fa98ccp-1f}, 0x1.152eaep-1f}, //     0  
  {{-0x1.fa9084p-1f,-0x1.fa9084p-1f}, 0x1.153eaep-1f}, //     0  
  {{-0x1.fa8836p-1f,-0x1.fa8836p-1f}, 0x1.154eaap-1f}, //     0  
  {{-0x1.fa7fe2p-1f,-0x1.fa7fe2p-1f}, 0x1.155ea6p-1f}, //     0  
  {{-0x1.fa7784p-1f,-0x1.fa7784p-1f}, 0x1.156eaap-1f}, //     0  
  {{-0x1.fa6f22p-1f,-0x1.fa6f22p-1f}, 0x1.157ea8p-1f}, //     0  
  {{-0x1.fa66b8p-1f,-0x1.fa66b8p-1f}, 0x1.158eaap-1f}, //     0  
  {{-0x1.fa5e4ap-1f,-0x1.fa5e4ap-1f}, 0x1.159ea6p-1f}, //     0  
  {{-0x1.fa55d4p-1f,-0x1.fa55d4p-1f}, 0x1.15aea6p-1f}, //     0  
  {{-0x1.fa4d58p-1f,-0x1.fa4d58p-1f}, 0x1.15bea4p-1f}, //     0  
  {{-0x1.fa44d4p-1f,-0x1.fa44d4p-1f}, 0x1.15cea4p-1f}, //     0  
  {{-0x1.fa3c4cp-1f,-0x1.fa3c4cp-1f}, 0x1.15dea2p-1f}, //     0  
  {{-0x1.fa33bap-1f,-0x1.fa33bap-1f}, 0x1.15eea4p-1f}, //     0  
  {{-0x1.fa2b24p-1f,-0x1.fa2b24p-1f}, 0x1.15fea2p-1f}, //     0  
  {{-0x1.fa2288p-1f,-0x1.fa2288p-1f}, 0x1.160ea0p-1f}, //     0  
  {{-0x1.fa19e6p-1f,-0x1.fa19e6p-1f}, 0x1.161e9cp-1f}, //     0  
  {{-0x1.fa113ap-1f,-0x1.fa113ap-1f}, 0x1.162e9ep-1f}, //     0  
  {{-0x1.fa088ap-1f,-0x1.fa088ap-1f}, 0x1.163e9ep-1f}, //     0  
  {{-0x1.f9ffd4p-1f,-0x1.f9ffd4p-1f}, 0x1.164e9ap-1f}, //     0  
  {{-0x1.f9f716p-1f,-0x1.f9f716p-1f}, 0x1.165e9ap-1f}, //     0  
  {{-0x1.f9ee52p-1f,-0x1.f9ee52p-1f}, 0x1.166e9ap-1f}, //     0  
  {{-0x1.f9e588p-1f,-0x1.f9e588p-1f}, 0x1.167e98p-1f}, //     0  
  {{-0x1.f9dcb6p-1f,-0x1.f9dcb6p-1f}, 0x1.168e98p-1f}, //     0  
  {{-0x1.f9d3dep-1f,-0x1.f9d3dep-1f}, 0x1.169e98p-1f}, //     0  
  {{-0x1.f9cb02p-1f,-0x1.f9cb02p-1f}, 0x1.16ae92p-1f}, //     0  
  {{-0x1.f9c21cp-1f,-0x1.f9c21cp-1f}, 0x1.16be94p-1f}, //     0  
  {{-0x1.f9b930p-1f,-0x1.f9b930p-1f}, 0x1.16ce94p-1f}, //     0  
  {{-0x1.f9b040p-1f,-0x1.f9b040p-1f}, 0x1.16de90p-1f}, //     0  
  {{-0x1.f9a746p-1f,-0x1.f9a746p-1f}, 0x1.16ee92p-1f}, //     0  
  {{-0x1.f99e48p-1f,-0x1.f99e48p-1f}, 0x1.16fe90p-1f}, //     0  
  {{-0x1.f99544p-1f,-0x1.f99544p-1f}, 0x1.170e8ep-1f}, //     0  
  {{-0x1.f98c36p-1f,-0x1.f98c36p-1f}, 0x1.171e90p-1f}, //     0  
  {{-0x1.f98324p-1f,-0x1.f98324p-1f}, 0x1.172e8ep-1f}, //     0  
  {{-0x1.f97a0cp-1f,-0x1.f97a0cp-1f}, 0x1.173e8cp-1f}, //     0  
  {{-0x1.f970eap-1f,-0x1.f970eap-1f}, 0x1.174e90p-1f}, //     0  
  {{-0x1.f967c6p-1f,-0x1.f967c6p-1f}, 0x1.175e8ap-1f}, //     0  
  {{-0x1.f95e9ap-1f,-0x1.f95e9ap-1f}, 0x1.176e88p-1f}, //     0  
  {{-0x1.f95568p-1f,-0x1.f95568p-1f}, 0x1.177e86p-1f}, //     0  
  {{-0x1.f94c2cp-1f,-0x1.f94c2cp-1f}, 0x1.178e88p-1f}, //     0  
  {{-0x1.f942eep-1f,-0x1.f942eep-1f}, 0x1.179e84p-1f}, //     0  
  {{-0x1.f939a4p-1f,-0x1.f939a4p-1f}, 0x1.17ae88p-1f}, //     0  
  {{-0x1.f93058p-1f,-0x1.f93058p-1f}, 0x1.17be86p-1f}, //     0  
  {{-0x1.f92704p-1f,-0x1.f92704p-1f}, 0x1.17ce84p-1f}, //     0  
  {{-0x1.f91dacp-1f,-0x1.f91dacp-1f}, 0x1.17de80p-1f}, //     0  
  {{-0x1.f91448p-1f,-0x1.f91448p-1f}, 0x1.17ee84p-1f}, //     0  
  {{-0x1.f90ae2p-1f,-0x1.f90ae2p-1f}, 0x1.17fe80p-1f}, //     0  
  {{-0x1.f90174p-1f,-0x1.f90174p-1f}, 0x1.180e80p-1f}, //     0  
  {{-0x1.f8f7fep-1f,-0x1.f8f7fep-1f}, 0x1.181e80p-1f}, //     0  
  {{-0x1.f8ee84p-1f,-0x1.f8ee84p-1f}, 0x1.182e7ep-1f}, //     0  
  {{-0x1.f8e502p-1f,-0x1.f8e502p-1f}, 0x1.183e7cp-1f}, //     0  
  {{-0x1.f8db78p-1f,-0x1.f8db78p-1f}, 0x1.184e7ep-1f}, //     0  
  {{-0x1.f8d1eap-1f,-0x1.f8d1eap-1f}, 0x1.185e7cp-1f}, //     0  
  {{-0x1.f8c856p-1f,-0x1.f8c856p-1f}, 0x1.186e78p-1f}, //     0  
  {{-0x1.f8bebap-1f,-0x1.f8bebap-1f}, 0x1.187e78p-1f}, //     0  
  {{-0x1.f8b516p-1f,-0x1.f8b516p-1f}, 0x1.188e78p-1f}, //     0  
  {{-0x1.f8ab6ep-1f,-0x1.f8ab6ep-1f}, 0x1.189e76p-1f}, //     0  
  {{-0x1.f8a1bcp-1f,-0x1.f8a1bcp-1f}, 0x1.18ae78p-1f}, //     0  
  {{-0x1.f89808p-1f,-0x1.f89808p-1f}, 0x1.18be74p-1f}, //     0  
  {{-0x1.f88e4ap-1f,-0x1.f88e4ap-1f}, 0x1.18ce74p-1f}, //     0  
  {{-0x1.f88488p-1f,-0x1.f88488p-1f}, 0x1.18de72p-1f}, //     0  
  {{-0x1.f87abcp-1f,-0x1.f87abcp-1f}, 0x1.18ee74p-1f}, //     0  
  {{-0x1.f870ecp-1f,-0x1.f870ecp-1f}, 0x1.18fe72p-1f}, //     0  
  {{-0x1.f86716p-1f,-0x1.f86716p-1f}, 0x1.190e6ep-1f}, //     0  
  {{-0x1.f85d36p-1f,-0x1.f85d36p-1f}, 0x1.191e70p-1f}, //     0  
  {{-0x1.f85352p-1f,-0x1.f85352p-1f}, 0x1.192e70p-1f}, //     0  
  {{-0x1.f84968p-1f,-0x1.f84968p-1f}, 0x1.193e6cp-1f}, //     0  
  {{-0x1.f83f76p-1f,-0x1.f83f76p-1f}, 0x1.194e6cp-1f}, //     0  
  {{-0x1.f8357ep-1f,-0x1.f8357ep-1f}, 0x1.195e6cp-1f}, //     0  
  {{-0x1.f82b80p-1f,-0x1.f82b80p-1f}, 0x1.196e68p-1f}, //     0  
  {{-0x1.f8217cp-1f,-0x1.f8217cp-1f}, 0x1.197e66p-1f}, //     0  
  {{-0x1.f8176ep-1f,-0x1.f8176ep-1f}, 0x1.198e68p-1f}, //     0  
  {{-0x1.f80d5cp-1f,-0x1.f80d5cp-1f}, 0x1.199e66p-1f}, //     0  
  {{-0x1.f80344p-1f,-0x1.f80344p-1f}, 0x1.19ae64p-1f}, //     0  
  {{-0x1.f7f922p-1f,-0x1.f7f922p-1f}, 0x1.19be66p-1f}, //     0  
  {{-0x1.f7eefcp-1f,-0x1.f7eefcp-1f}, 0x1.19ce64p-1f}, //     0  
  {{-0x1.f7e4d0p-1f,-0x1.f7e4d0p-1f}, 0x1.19de62p-1f}, //     0  
  {{-0x1.f7da9ap-1f,-0x1.f7da9ap-1f}, 0x1.19ee64p-1f}, //     0  
  {{-0x1.f7d062p-1f,-0x1.f7d062p-1f}, 0x1.19fe60p-1f}, //     0  
  {{-0x1.f7c622p-1f,-0x1.f7c622p-1f}, 0x1.1a0e5ep-1f}, //     0  
  {{-0x1.f7bbdcp-1f,-0x1.f7bbdcp-1f}, 0x1.1a1e5cp-1f}, //     0  
  {{-0x1.f7b18cp-1f,-0x1.f7b18cp-1f}, 0x1.1a2e5ep-1f}, //     0  
  {{-0x1.f7a738p-1f,-0x1.f7a738p-1f}, 0x1.1a3e5cp-1f}, //     0  
  {{-0x1.f79cdcp-1f,-0x1.f79cdcp-1f}, 0x1.1a4e5cp-1f}, //     0  
  {{-0x1.f7927ap-1f,-0x1.f7927ap-1f}, 0x1.1a5e5cp-1f}, //     0  
  {{-0x1.f78812p-1f,-0x1.f78812p-1f}, 0x1.1a6e5ap-1f}, //     0  
  {{-0x1.f77da4p-1f,-0x1.f77da4p-1f}, 0x1.1a7e56p-1f}, //     0  
  {{-0x1.f7732cp-1f,-0x1.f7732cp-1f}, 0x1.1a8e5ap-1f}, //     0  
  {{-0x1.f768b0p-1f,-0x1.f768b0p-1f}, 0x1.1a9e58p-1f}, //     0  
  {{-0x1.f75e2ep-1f,-0x1.f75e2ep-1f}, 0x1.1aae56p-1f}, //     0  
  {{-0x1.f753a6p-1f,-0x1.f753a6p-1f}, 0x1.1abe52p-1f}, //     0  
  {{-0x1.f74914p-1f,-0x1.f74914p-1f}, 0x1.1ace54p-1f}, //     0  
  {{-0x1.f73e7ep-1f,-0x1.f73e7ep-1f}, 0x1.1ade52p-1f}, //     0  
  {{-0x1.f733e0p-1f,-0x1.f733e0p-1f}, 0x1.1aee52p-1f}, //     0  
  {{-0x1.f7293ep-1f,-0x1.f7293ep-1f}, 0x1.1afe4ep-1f}, //     0  
  {{-0x1.f71e92p-1f,-0x1.f71e92p-1f}, 0x1.1b0e50p-1f}, //     0  
  {{-0x1.f713e0p-1f,-0x1.f713e0p-1f}, 0x1.1b1e50p-1f}, //     0  
  {{-0x1.f7092ap-1f,-0x1.f7092ap-1f}, 0x1.1b2e4cp-1f}, //     0  
  {{-0x1.f6fe6ap-1f,-0x1.f6fe6ap-1f}, 0x1.1b3e4cp-1f}, //     0  
  {{-0x1.f6f3a4p-1f,-0x1.f6f3a4p-1f}, 0x1.1b4e4cp-1f}, //     0  
  {{-0x1.f6e8dap-1f,-0x1.f6e8dap-1f}, 0x1.1b5e4ap-1f}, //     0  
  {{-0x1.f6de06p-1f,-0x1.f6de06p-1f}, 0x1.1b6e4ap-1f}, //     0  
  {{-0x1.f6d32ep-1f,-0x1.f6d32ep-1f}, 0x1.1b7e48p-1f}, //     0  
  {{-0x1.f6c84ep-1f,-0x1.f6c84ep-1f}, 0x1.1b8e48p-1f}, //     0  
  {{-0x1.f6bd68p-1f,-0x1.f6bd68p-1f}, 0x1.1b9e46p-1f}, //     0  
  {{-0x1.f6b27ap-1f,-0x1.f6b27ap-1f}, 0x1.1bae46p-1f}, //     0  
  {{-0x1.f6a786p-1f,-0x1.f6a786p-1f}, 0x1.1bbe46p-1f}, //     0  
  {{-0x1.f69c8cp-1f,-0x1.f69c8cp-1f}, 0x1.1bce44p-1f}, //     0  
  {{-0x1.f6918cp-1f,-0x1.f6918cp-1f}, 0x1.1bde42p-1f}, //     0  
  {{-0x1.f68684p-1f,-0x1.f68684p-1f}, 0x1.1bee42p-1f}, //     0  
  {{-0x1.f67b76p-1f,-0x1.f67b76p-1f}, 0x1.1bfe40p-1f}, //     0  
  {{-0x1.f67062p-1f,-0x1.f67062p-1f}, 0x1.1c0e3ep-1f}, //     0  
  {{-0x1.f66544p-1f,-0x1.f66544p-1f}, 0x1.1c1e40p-1f}, //     0  
  {{-0x1.f65a22p-1f,-0x1.f65a22p-1f}, 0x1.1c2e3ep-1f}, //     0  
  {{-0x1.f64efap-1f,-0x1.f64efap-1f}, 0x1.1c3e3cp-1f}, //     0  
  {{-0x1.f643cap-1f,-0x1.f643cap-1f}, 0x1.1c4e3cp-1f}, //     0  
  {{-0x1.f63894p-1f,-0x1.f63894p-1f}, 0x1.1c5e3cp-1f}, //     0  
  {{-0x1.f62d58p-1f,-0x1.f62d58p-1f}, 0x1.1c6e38p-1f}, //     0  
  {{-0x1.f62214p-1f,-0x1.f62214p-1f}, 0x1.1c7e38p-1f}, //     0  
  {{-0x1.f616c8p-1f,-0x1.f616c8p-1f}, 0x1.1c8e3ap-1f}, //     0  
  {{-0x1.f60b78p-1f,-0x1.f60b78p-1f}, 0x1.1c9e38p-1f}, //     0  
  {{-0x1.f60022p-1f,-0x1.f60022p-1f}, 0x1.1cae34p-1f}, //     0  
  {{-0x1.f5f4c4p-1f,-0x1.f5f4c4p-1f}, 0x1.1cbe34p-1f}, //     0  
  {{-0x1.f5e95ep-1f,-0x1.f5e95ep-1f}, 0x1.1cce34p-1f}, //     0  
  {{-0x1.f5ddf4p-1f,-0x1.f5ddf4p-1f}, 0x1.1cde30p-1f}, //     0  
  {{-0x1.f5d280p-1f,-0x1.f5d280p-1f}, 0x1.1cee32p-1f}, //     0  
  {{-0x1.f5c70ap-1f,-0x1.f5c70ap-1f}, 0x1.1cfe2ep-1f}, //     0  
  {{-0x1.f5bb88p-1f,-0x1.f5bb88p-1f}, 0x1.1d0e30p-1f}, //     0  
  {{-0x1.f5b002p-1f,-0x1.f5b002p-1f}, 0x1.1d1e2ep-1f}, //     0  
  {{-0x1.f5a476p-1f,-0x1.f5a476p-1f}, 0x1.1d2e2cp-1f}, //     0  
  {{-0x1.f598e0p-1f,-0x1.f598e0p-1f}, 0x1.1d3e2ep-1f}, //     0  
  {{-0x1.f58d46p-1f,-0x1.f58d46p-1f}, 0x1.1d4e2ep-1f}, //     0  
  {{-0x1.f581a6p-1f,-0x1.f581a6p-1f}, 0x1.1d5e2ap-1f}, //     0  
  {{-0x1.f57600p-1f,-0x1.f57600p-1f}, 0x1.1d6e28p-1f}, //     0  
  {{-0x1.f56a50p-1f,-0x1.f56a50p-1f}, 0x1.1d7e28p-1f}, //     0  
  {{-0x1.f55e9ap-1f,-0x1.f55e9ap-1f}, 0x1.1d8e2ap-1f}, //     0  
  {{-0x1.f552e0p-1f,-0x1.f552e0p-1f}, 0x1.1d9e26p-1f}, //     0  
  {{-0x1.f5471cp-1f,-0x1.f5471cp-1f}, 0x1.1dae28p-1f}, //     0  
  {{-0x1.f53b54p-1f,-0x1.f53b54p-1f}, 0x1.1dbe24p-1f}, //     0  
  {{-0x1.f52f84p-1f,-0x1.f52f84p-1f}, 0x1.1dce24p-1f}, //     0  
  {{-0x1.f523aep-1f,-0x1.f523aep-1f}, 0x1.1dde22p-1f}, //     0  
  {{-0x1.f517d0p-1f,-0x1.f517d0p-1f}, 0x1.1dee22p-1f}, //     0  
  {{-0x1.f50becp-1f,-0x1.f50becp-1f}, 0x1.1dfe22p-1f}, //     0  
  {{-0x1.f50002p-1f,-0x1.f50002p-1f}, 0x1.1e0e20p-1f}, //     0  
  {{-0x1.f4f412p-1f,-0x1.f4f412p-1f}, 0x1.1e1e1ep-1f}, //     0  
  {{-0x1.f4e818p-1f,-0x1.f4e818p-1f}, 0x1.1e2e1ep-1f}, //     0  
  {{-0x1.f4dc1ap-1f,-0x1.f4dc1ap-1f}, 0x1.1e3e1cp-1f}, //     0  
  {{-0x1.f4d016p-1f,-0x1.f4d016p-1f}, 0x1.1e4e1ap-1f}, //     0  
  {{-0x1.f4c40ap-1f,-0x1.f4c40ap-1f}, 0x1.1e5e1ap-1f}, //     0  
  {{-0x1.f4b7f6p-1f,-0x1.f4b7f6p-1f}, 0x1.1e6e1ap-1f}, //     0  
  {{-0x1.f4abdcp-1f,-0x1.f4abdcp-1f}, 0x1.1e7e1ap-1f}, //     0  
  {{-0x1.f49fbcp-1f,-0x1.f49fbcp-1f}, 0x1.1e8e18p-1f}, //     0  
  {{-0x1.f49396p-1f,-0x1.f49396p-1f}, 0x1.1e9e16p-1f}, //     0  
  {{-0x1.f48766p-1f,-0x1.f48766p-1f}, 0x1.1eae18p-1f}, //     0  
  {{-0x1.f47b34p-1f,-0x1.f47b34p-1f}, 0x1.1ebe14p-1f}, //     0  
  {{-0x1.f46ef8p-1f,-0x1.f46ef8p-1f}, 0x1.1ece14p-1f}, //     0  
  {{-0x1.f462b8p-1f,-0x1.f462b8p-1f}, 0x1.1ede10p-1f}, //     0  
  {{-0x1.f4566ep-1f,-0x1.f4566ep-1f}, 0x1.1eee12p-1f}, //     0  
  {{-0x1.f44a20p-1f,-0x1.f44a20p-1f}, 0x1.1efe0ep-1f}, //     0  
  {{-0x1.f43dcap-1f,-0x1.f43dcap-1f}, 0x1.1f0e0ep-1f}, //     0  
  {{-0x1.f4316ep-1f,-0x1.f4316ep-1f}, 0x1.1f1e0cp-1f}, //     0  
  {{-0x1.f42508p-1f,-0x1.f42508p-1f}, 0x1.1f2e0ep-1f}, //     0  
  {{-0x1.f4189ep-1f,-0x1.f4189ep-1f}, 0x1.1f3e0ep-1f}, //     0  
  {{-0x1.f40c2ep-1f,-0x1.f40c2ep-1f}, 0x1.1f4e0cp-1f}, //     0  
  {{-0x1.f3ffb8p-1f,-0x1.f3ffb8p-1f}, 0x1.1f5e08p-1f}, //     0  
  {{-0x1.f3f338p-1f,-0x1.f3f338p-1f}, 0x1.1f6e0ap-1f}, //     0  
  {{-0x1.f3e6b4p-1f,-0x1.f3e6b4p-1f}, 0x1.1f7e06p-1f}, //     0  
  {{-0x1.f3da28p-1f,-0x1.f3da28p-1f}, 0x1.1f8e06p-1f}, //     0  
  {{-0x1.f3cd96p-1f,-0x1.f3cd96p-1f}, 0x1.1f9e04p-1f}, //     0  
  {{-0x1.f3c0fap-1f,-0x1.f3c0fap-1f}, 0x1.1fae06p-1f}, //     0  
  {{-0x1.f3b45cp-1f,-0x1.f3b45cp-1f}, 0x1.1fbe04p-1f}, //     0  
  {{-0x1.f3a7b4p-1f,-0x1.f3a7b4p-1f}, 0x1.1fce04p-1f}, //     0  
  {{-0x1.f39b06p-1f,-0x1.f39b06p-1f}, 0x1.1fde04p-1f}, //     0  
  {{-0x1.f38e52p-1f,-0x1.f38e52p-1f}, 0x1.1fee02p-1f}, //     0  
  {{-0x1.f38198p-1f,-0x1.f38198p-1f}, 0x1.1ffe00p-1f}, //     0  
  {{-0x1.f374d6p-1f,-0x1.f374d6p-1f}, 0x1.200e00p-1f}, //     0  
  {{-0x1.f3680cp-1f,-0x1.f3680cp-1f}, 0x1.201e00p-1f}, //     0  
  {{-0x1.f35b3ep-1f,-0x1.f35b3ep-1f}, 0x1.202dfep-1f}, //     0  
  {{-0x1.f34e68p-1f,-0x1.f34e68p-1f}, 0x1.203dfcp-1f}, //     0  
  {{-0x1.f3418cp-1f,-0x1.f3418cp-1f}, 0x1.204dfap-1f}, //     0  
  {{-0x1.f334aap-1f,-0x1.f334aap-1f}, 0x1.205df8p-1f}, //     0  
  {{-0x1.f327bcp-1f,-0x1.f327bcp-1f}, 0x1.206dfcp-1f}, //     0  
  {{-0x1.f31acep-1f,-0x1.f31acep-1f}, 0x1.207df6p-1f}, //     0  
  {{-0x1.f30dd6p-1f,-0x1.f30dd6p-1f}, 0x1.208df6p-1f}, //     0  
  {{-0x1.f300d8p-1f,-0x1.f300d8p-1f}, 0x1.209df4p-1f}, //     0  
  {{-0x1.f2f3d0p-1f,-0x1.f2f3d0p-1f}, 0x1.20adf6p-1f}, //     0  
  {{-0x1.f2e6c6p-1f,-0x1.f2e6c6p-1f}, 0x1.20bdf4p-1f}, //     0  
  {{-0x1.f2d9b2p-1f,-0x1.f2d9b2p-1f}, 0x1.20cdf4p-1f}, //     0  
  {{-0x1.f2cc9ap-1f,-0x1.f2cc9ap-1f}, 0x1.20ddf0p-1f}, //     0  
  {{-0x1.f2bf78p-1f,-0x1.f2bf78p-1f}, 0x1.20edf2p-1f}, //     0  
  {{-0x1.f2b252p-1f,-0x1.f2b252p-1f}, 0x1.20fdf0p-1f}, //     0  
  {{-0x1.f2a524p-1f,-0x1.f2a524p-1f}, 0x1.210deep-1f}, //     0  
  {{-0x1.f297f0p-1f,-0x1.f297f0p-1f}, 0x1.211decp-1f}, //     0  
  {{-0x1.f28ab2p-1f,-0x1.f28ab2p-1f}, 0x1.212df0p-1f}, //     0  
  {{-0x1.f27d72p-1f,-0x1.f27d72p-1f}, 0x1.213decp-1f}, //     0  
  {{-0x1.f27028p-1f,-0x1.f27028p-1f}, 0x1.214decp-1f}, //     0  
  {{-0x1.f262dap-1f,-0x1.f262dap-1f}, 0x1.215de8p-1f}, //     0  
  {{-0x1.f25580p-1f,-0x1.f25580p-1f}, 0x1.216decp-1f}, //     0  
  {{-0x1.f24824p-1f,-0x1.f24824p-1f}, 0x1.217deap-1f}, //     0  
  {{-0x1.f23ac0p-1f,-0x1.f23ac0p-1f}, 0x1.218de8p-1f}, //     0  
  {{-0x1.f22d56p-1f,-0x1.f22d56p-1f}, 0x1.219de6p-1f}, //     0  
  {{-0x1.f21fe2p-1f,-0x1.f21fe2p-1f}, 0x1.21ade8p-1f}, //     0  
  {{-0x1.f2126cp-1f,-0x1.f2126cp-1f}, 0x1.21bde4p-1f}, //     0  
  {{-0x1.f204ecp-1f,-0x1.f204ecp-1f}, 0x1.21cde4p-1f}, //     0  
  {{-0x1.f1f768p-1f,-0x1.f1f768p-1f}, 0x1.21dde0p-1f}, //     0  
  {{-0x1.f1e9d8p-1f,-0x1.f1e9d8p-1f}, 0x1.21ede4p-1f}, //     0  
  {{-0x1.f1dc46p-1f,-0x1.f1dc46p-1f}, 0x1.21fde0p-1f}, //     0  
  {{-0x1.f1ceacp-1f,-0x1.f1ceacp-1f}, 0x1.220ddep-1f}, //     0  
  {{-0x1.f1c10cp-1f,-0x1.f1c10cp-1f}, 0x1.221ddcp-1f}, //     0  
  {{-0x1.f1b360p-1f,-0x1.f1b360p-1f}, 0x1.222de0p-1f}, //     0  
  {{-0x1.f1a5b4p-1f,-0x1.f1a5b4p-1f}, 0x1.223ddcp-1f}, //     0  
  {{-0x1.f197fep-1f,-0x1.f197fep-1f}, 0x1.224ddcp-1f}, //     0  
  {{-0x1.f18a42p-1f,-0x1.f18a42p-1f}, 0x1.225ddap-1f}, //     0  
  {{-0x1.f17c7cp-1f,-0x1.f17c7cp-1f}, 0x1.226ddcp-1f}, //     0  
  {{-0x1.f16eb4p-1f,-0x1.f16eb4p-1f}, 0x1.227dd8p-1f}, //     0  
  {{-0x1.f160e4p-1f,-0x1.f160e4p-1f}, 0x1.228dd6p-1f}, //     0  
  {{-0x1.f1530cp-1f,-0x1.f1530cp-1f}, 0x1.229dd6p-1f}, //     0  
  {{-0x1.f1452ep-1f,-0x1.f1452ep-1f}, 0x1.22add4p-1f}, //     0  
  {{-0x1.f13748p-1f,-0x1.f13748p-1f}, 0x1.22bdd4p-1f}, //     0  
  {{-0x1.f1295cp-1f,-0x1.f1295cp-1f}, 0x1.22cdd4p-1f}, //     0  
  {{-0x1.f11b6ap-1f,-0x1.f11b6ap-1f}, 0x1.22ddd2p-1f}, //     0  
  {{-0x1.f10d70p-1f,-0x1.f10d70p-1f}, 0x1.22edd2p-1f}, //     0  
  {{-0x1.f0ff70p-1f,-0x1.f0ff70p-1f}, 0x1.22fdd0p-1f}, //     0  
  {{-0x1.f0f168p-1f,-0x1.f0f168p-1f}, 0x1.230dd0p-1f}, //     0  
  {{-0x1.f0e35ap-1f,-0x1.f0e35ap-1f}, 0x1.231dcep-1f}, //     0  
  {{-0x1.f0d546p-1f,-0x1.f0d546p-1f}, 0x1.232dccp-1f}, //     0  
  {{-0x1.f0c72ap-1f,-0x1.f0c72ap-1f}, 0x1.233dccp-1f}, //     0  
  {{-0x1.f0b906p-1f,-0x1.f0b906p-1f}, 0x1.234dccp-1f}, //     0  
  {{-0x1.f0aadep-1f,-0x1.f0aadep-1f}, 0x1.235dcap-1f}, //     0  
  {{-0x1.f09cacp-1f,-0x1.f09cacp-1f}, 0x1.236dcap-1f}, //     0  
  {{-0x1.f08e76p-1f,-0x1.f08e76p-1f}, 0x1.237dc8p-1f}, //     0  
  {{-0x1.f08036p-1f,-0x1.f08036p-1f}, 0x1.238dcap-1f}, //     0  
  {{-0x1.f071f4p-1f,-0x1.f071f4p-1f}, 0x1.239dc6p-1f}, //     0  
  {{-0x1.f063a8p-1f,-0x1.f063a8p-1f}, 0x1.23adc6p-1f}, //     0  
  {{-0x1.f05556p-1f,-0x1.f05556p-1f}, 0x1.23bdc4p-1f}, //     0  
  {{-0x1.f046fcp-1f,-0x1.f046fcp-1f}, 0x1.23cdc4p-1f}, //     0  
  {{-0x1.f0389cp-1f,-0x1.f0389cp-1f}, 0x1.23ddc2p-1f}, //     0  
  {{-0x1.f02a34p-1f,-0x1.f02a34p-1f}, 0x1.23edc4p-1f}, //     0  
  {{-0x1.f01bc8p-1f,-0x1.f01bc8p-1f}, 0x1.23fdc0p-1f}, //     0  
  {{-0x1.f00d54p-1f,-0x1.f00d54p-1f}, 0x1.240dbep-1f}, //     0  
  {{-0x1.effedap-1f,-0x1.effedap-1f}, 0x1.241dbcp-1f}, //     0  
  {{-0x1.eff054p-1f,-0x1.eff054p-1f}, 0x1.242dc0p-1f}, //     0  
  {{-0x1.efe1ccp-1f,-0x1.efe1ccp-1f}, 0x1.243dbcp-1f}, //     0  
  {{-0x1.efd33cp-1f,-0x1.efd33cp-1f}, 0x1.244dbcp-1f}, //     0  
  {{-0x1.efc4a6p-1f,-0x1.efc4a6p-1f}, 0x1.245dbap-1f}, //     0  
  {{-0x1.efb606p-1f,-0x1.efb606p-1f}, 0x1.246dbcp-1f}, //     0  
  {{-0x1.efa764p-1f,-0x1.efa764p-1f}, 0x1.247db8p-1f}, //     0  
  {{-0x1.ef98b8p-1f,-0x1.ef98b8p-1f}, 0x1.248db8p-1f}, //     0  
  {{-0x1.ef8a06p-1f,-0x1.ef8a06p-1f}, 0x1.249db6p-1f}, //     0  
  {{-0x1.ef7b4cp-1f,-0x1.ef7b4cp-1f}, 0x1.24adb6p-1f}, //     0  
  {{-0x1.ef6c8cp-1f,-0x1.ef6c8cp-1f}, 0x1.24bdb6p-1f}, //     0  
  {{-0x1.ef5dc6p-1f,-0x1.ef5dc6p-1f}, 0x1.24cdb4p-1f}, //     0  
  {{-0x1.ef4efap-1f,-0x1.ef4efap-1f}, 0x1.24ddb2p-1f}, //     0  
  {{-0x1.ef4024p-1f,-0x1.ef4024p-1f}, 0x1.24edb2p-1f}, //     0  
  {{-0x1.ef314ap-1f,-0x1.ef314ap-1f}, 0x1.24fdb0p-1f}, //     0  
  {{-0x1.ef2266p-1f,-0x1.ef2266p-1f}, 0x1.250db0p-1f}, //     0  
  {{-0x1.ef137ep-1f,-0x1.ef137ep-1f}, 0x1.251daep-1f}, //     0  
  {{-0x1.ef048ep-1f,-0x1.ef048ep-1f}, 0x1.252daep-1f}, //     0  
  {{-0x1.eef598p-1f,-0x1.eef598p-1f}, 0x1.253dacp-1f}, //     0  
  {{-0x1.eee698p-1f,-0x1.eee698p-1f}, 0x1.254dacp-1f}, //     0  
  {{-0x1.eed794p-1f,-0x1.eed794p-1f}, 0x1.255dacp-1f}, //     0  
  {{-0x1.eec888p-1f,-0x1.eec888p-1f}, 0x1.256daap-1f}, //     0  
  {{-0x1.eeb978p-1f,-0x1.eeb978p-1f}, 0x1.257da8p-1f}, //     0  
  {{-0x1.eeaa5ep-1f,-0x1.eeaa5ep-1f}, 0x1.258da8p-1f}, //     0  
  {{-0x1.ee9b3ep-1f,-0x1.ee9b3ep-1f}, 0x1.259da6p-1f}, //     0  
  {{-0x1.ee8c16p-1f,-0x1.ee8c16p-1f}, 0x1.25ada6p-1f}, //     0  
  {{-0x1.ee7ceap-1f,-0x1.ee7ceap-1f}, 0x1.25bda4p-1f}, //     0  
  {{-0x1.ee6db4p-1f,-0x1.ee6db4p-1f}, 0x1.25cda4p-1f}, //     0  
  {{-0x1.ee5e7ap-1f,-0x1.ee5e7ap-1f}, 0x1.25dda2p-1f}, //     0  
  {{-0x1.ee4f36p-1f,-0x1.ee4f36p-1f}, 0x1.25eda2p-1f}, //     0  
  {{-0x1.ee3feep-1f,-0x1.ee3feep-1f}, 0x1.25fda0p-1f}, //     0  
  {{-0x1.ee309cp-1f,-0x1.ee309cp-1f}, 0x1.260da0p-1f}, //     0  
  {{-0x1.ee2146p-1f,-0x1.ee2146p-1f}, 0x1.261d9ep-1f}, //     0  
  {{-0x1.ee11e6p-1f,-0x1.ee11e6p-1f}, 0x1.262da0p-1f}, //     0  
  {{-0x1.ee0282p-1f,-0x1.ee0282p-1f}, 0x1.263d9ep-1f}, //     0  
  {{-0x1.edf316p-1f,-0x1.edf316p-1f}, 0x1.264d9cp-1f}, //     0  
  {{-0x1.ede3a4p-1f,-0x1.ede3a4p-1f}, 0x1.265d9cp-1f}, //     0  
  {{-0x1.edd42cp-1f,-0x1.edd42cp-1f}, 0x1.266d98p-1f}, //     0  
  {{-0x1.edc4acp-1f,-0x1.edc4acp-1f}, 0x1.267d96p-1f}, //     0  
  {{-0x1.edb522p-1f,-0x1.edb522p-1f}, 0x1.268d98p-1f}, //     0  
  {{-0x1.eda594p-1f,-0x1.eda594p-1f}, 0x1.269d98p-1f}, //     0  
  {{-0x1.ed9600p-1f,-0x1.ed9600p-1f}, 0x1.26ad96p-1f}, //     0  
  {{-0x1.ed8664p-1f,-0x1.ed8664p-1f}, 0x1.26bd94p-1f}, //     0  
  {{-0x1.ed76c0p-1f,-0x1.ed76c0p-1f}, 0x1.26cd94p-1f}, //     0  
  {{-0x1.ed6716p-1f,-0x1.ed6716p-1f}, 0x1.26dd94p-1f}, //     0  
  {{-0x1.ed5766p-1f,-0x1.ed5766p-1f}, 0x1.26ed92p-1f}, //     0  
  {{-0x1.ed47aep-1f,-0x1.ed47aep-1f}, 0x1.26fd90p-1f}, //     0  
  {{-0x1.ed37f0p-1f,-0x1.ed37f0p-1f}, 0x1.270d90p-1f}, //     0  
  {{-0x1.ed282cp-1f,-0x1.ed282cp-1f}, 0x1.271d8cp-1f}, //     0  
  {{-0x1.ed185cp-1f,-0x1.ed185cp-1f}, 0x1.272d90p-1f}, //     0  
  {{-0x1.ed088ap-1f,-0x1.ed088ap-1f}, 0x1.273d8cp-1f}, //     0  
  {{-0x1.ecf8b0p-1f,-0x1.ecf8b0p-1f}, 0x1.274d8cp-1f}, //     0  
  {{-0x1.ece8d0p-1f,-0x1.ece8d0p-1f}, 0x1.275d8ap-1f}, //     0  
  {{-0x1.ecd8e8p-1f,-0x1.ecd8e8p-1f}, 0x1.276d88p-1f}, //     0  
  {{-0x1.ecc8fap-1f,-0x1.ecc8fap-1f}, 0x1.277d86p-1f}, //     0  
  {{-0x1.ecb900p-1f,-0x1.ecb900p-1f}, 0x1.278d8ap-1f}, //     0  
  {{-0x1.eca906p-1f,-0x1.eca906p-1f}, 0x1.279d86p-1f}, //     0  
  {{-0x1.ec9902p-1f,-0x1.ec9902p-1f}, 0x1.27ad84p-1f}, //     0  
  {{-0x1.ec88f8p-1f,-0x1.ec88f8p-1f}, 0x1.27bd82p-1f}, //     0  
  {{-0x1.ec78e4p-1f,-0x1.ec78e4p-1f}, 0x1.27cd84p-1f}, //     0  
  {{-0x1.ec68ccp-1f,-0x1.ec68ccp-1f}, 0x1.27dd82p-1f}, //     0  
  {{-0x1.ec58acp-1f,-0x1.ec58acp-1f}, 0x1.27ed82p-1f}, //     0  
  {{-0x1.ec4886p-1f,-0x1.ec4886p-1f}, 0x1.27fd80p-1f}, //     0  
  {{-0x1.ec385ap-1f,-0x1.ec385ap-1f}, 0x1.280d7ep-1f}, //     0  
  {{-0x1.ec2826p-1f,-0x1.ec2826p-1f}, 0x1.281d7ep-1f}, //     0  
  {{-0x1.ec17e8p-1f,-0x1.ec17e8p-1f}, 0x1.282d7ep-1f}, //     0  
  {{-0x1.ec07a8p-1f,-0x1.ec07a8p-1f}, 0x1.283d7cp-1f}, //     0  
  {{-0x1.ebf75ep-1f,-0x1.ebf75ep-1f}, 0x1.284d7cp-1f}, //     0  
  {{-0x1.ebe70ep-1f,-0x1.ebe70ep-1f}, 0x1.285d7ap-1f}, //     0  
  {{-0x1.ebd6b8p-1f,-0x1.ebd6b8p-1f}, 0x1.286d78p-1f}, //     0  
  {{-0x1.ebc65ap-1f,-0x1.ebc65ap-1f}, 0x1.287d78p-1f}, //     0  
  {{-0x1.ebb5f4p-1f,-0x1.ebb5f4p-1f}, 0x1.288d78p-1f}, //     0  
  {{-0x1.eba588p-1f,-0x1.eba588p-1f}, 0x1.289d76p-1f}, //     0  
  {{-0x1.eb9516p-1f,-0x1.eb9514p-1f}, 0x1.28ad76p-1f}, //     1  
  {{-0x1.eb849cp-1f,-0x1.eb849cp-1f}, 0x1.28bd74p-1f}, //     0  
  {{-0x1.eb741ap-1f,-0x1.eb741ap-1f}, 0x1.28cd74p-1f}, //     0  
  {{-0x1.eb6392p-1f,-0x1.eb6392p-1f}, 0x1.28dd74p-1f}, //     0  
  {{-0x1.eb5304p-1f,-0x1.eb5304p-1f}, 0x1.28ed72p-1f}, //     0  
  {{-0x1.eb4270p-1f,-0x1.eb4270p-1f}, 0x1.28fd70p-1f}, //     0  
  {{-0x1.eb31d2p-1f,-0x1.eb31d2p-1f}, 0x1.290d70p-1f}, //     0  
  {{-0x1.eb2130p-1f,-0x1.eb2130p-1f}, 0x1.291d6ep-1f}, //     0  
  {{-0x1.eb1084p-1f,-0x1.eb1084p-1f}, 0x1.292d6ep-1f}, //     0  
  {{-0x1.eaffd4p-1f,-0x1.eaffd4p-1f}, 0x1.293d6cp-1f}, //     0  
  {{-0x1.eaef1ap-1f,-0x1.eaef1ap-1f}, 0x1.294d6cp-1f}, //     0  
  {{-0x1.eade5cp-1f,-0x1.eade5cp-1f}, 0x1.295d6ap-1f}, //     0  
  {{-0x1.eacd98p-1f,-0x1.eacd96p-1f}, 0x1.296d68p-1f}, //     1  
  {{-0x1.eabccap-1f,-0x1.eabccap-1f}, 0x1.297d66p-1f}, //     0  
  {{-0x1.eaabf2p-1f,-0x1.eaabf2p-1f}, 0x1.298d6ap-1f}, //     0  
  {{-0x1.ea9b18p-1f,-0x1.ea9b18p-1f}, 0x1.299d68p-1f}, //     0  
  {{-0x1.ea8a36p-1f,-0x1.ea8a36p-1f}, 0x1.29ad66p-1f}, //     0  
  {{-0x1.ea794ep-1f,-0x1.ea794ep-1f}, 0x1.29bd64p-1f}, //     0  
  {{-0x1.ea6860p-1f,-0x1.ea685ep-1f}, 0x1.29cd62p-1f}, //     1  
  {{-0x1.ea5768p-1f,-0x1.ea5768p-1f}, 0x1.29dd60p-1f}, //     0  
  {{-0x1.ea4668p-1f,-0x1.ea4668p-1f}, 0x1.29ed62p-1f}, //     0  
  {{-0x1.ea3564p-1f,-0x1.ea3564p-1f}, 0x1.29fd60p-1f}, //     0  
  {{-0x1.ea2456p-1f,-0x1.ea2456p-1f}, 0x1.2a0d60p-1f}, //     0  
  {{-0x1.ea1344p-1f,-0x1.ea1344p-1f}, 0x1.2a1d5ep-1f}, //     0  
  {{-0x1.ea022ap-1f,-0x1.ea022ap-1f}, 0x1.2a2d5ep-1f}, //     0  
  {{-0x1.e9f10ap-1f,-0x1.e9f10ap-1f}, 0x1.2a3d5cp-1f}, //     0  
  {{-0x1.e9dfe0p-1f,-0x1.e9dfe0p-1f}, 0x1.2a4d5cp-1f}, //     0  
  {{-0x1.e9ceb2p-1f,-0x1.e9ceb2p-1f}, 0x1.2a5d5ap-1f}, //     0  
  {{-0x1.e9bd7cp-1f,-0x1.e9bd7cp-1f}, 0x1.2a6d5ap-1f}, //     0  
  {{-0x1.e9ac40p-1f,-0x1.e9ac40p-1f}, 0x1.2a7d58p-1f}, //     0  
  {{-0x1.e99afep-1f,-0x1.e99afcp-1f}, 0x1.2a8d56p-1f}, //     1  
  {{-0x1.e989b2p-1f,-0x1.e989b0p-1f}, 0x1.2a9d56p-1f}, //     1  
  {{-0x1.e9785ep-1f,-0x1.e9785ep-1f}, 0x1.2aad56p-1f}, //     0  
  {{-0x1.e96706p-1f,-0x1.e96706p-1f}, 0x1.2abd54p-1f}, //     0  
  {{-0x1.e955a6p-1f,-0x1.e955a6p-1f}, 0x1.2acd54p-1f}, //     0  
  {{-0x1.e94440p-1f,-0x1.e94440p-1f}, 0x1.2add52p-1f}, //     0  
  {{-0x1.e932d2p-1f,-0x1.e932d2p-1f}, 0x1.2aed50p-1f}, //     0  
  {{-0x1.e9215ep-1f,-0x1.e9215ep-1f}, 0x1.2afd4ep-1f}, //     0  
  {{-0x1.e90fdep-1f,-0x1.e90fdep-1f}, 0x1.2b0d52p-1f}, //     0  
  {{-0x1.e8fe5ep-1f,-0x1.e8fe5ep-1f}, 0x1.2b1d4ep-1f}, //     0  
  {{-0x1.e8ecd2p-1f,-0x1.e8ecd2p-1f}, 0x1.2b2d4ep-1f}, //     0  
  {{-0x1.e8db46p-1f,-0x1.e8db44p-1f}, 0x1.2b3d4ap-1f}, //     1  
  {{-0x1.e8c9aap-1f,-0x1.e8c9aap-1f}, 0x1.2b4d4cp-1f}, //     0  
  {{-0x1.e8b80cp-1f,-0x1.e8b80cp-1f}, 0x1.2b5d4ap-1f}, //     0  
  {{-0x1.e8a664p-1f,-0x1.e8a664p-1f}, 0x1.2b6d4ap-1f}, //     0  
  {{-0x1.e894bap-1f,-0x1.e894b8p-1f}, 0x1.2b7d48p-1f}, //     1  
  {{-0x1.e88304p-1f,-0x1.e88304p-1f}, 0x1.2b8d48p-1f}, //     0  
  {{-0x1.e8714ap-1f,-0x1.e8714ap-1f}, 0x1.2b9d46p-1f}, //     0  
  {{-0x1.e85f8ap-1f,-0x1.e85f88p-1f}, 0x1.2bad44p-1f}, //     1  
  {{-0x1.e84dc2p-1f,-0x1.e84dc0p-1f}, 0x1.2bbd42p-1f}, //     1  
  {{-0x1.e83becp-1f,-0x1.e83becp-1f}, 0x1.2bcd46p-1f}, //     0  
  {{-0x1.e82a16p-1f,-0x1.e82a16p-1f}, 0x1.2bdd44p-1f}, //     0  
  {{-0x1.e81838p-1f,-0x1.e81838p-1f}, 0x1.2bed42p-1f}, //     0  
  {{-0x1.e80654p-1f,-0x1.e80654p-1f}, 0x1.2bfd40p-1f}, //     0  
  {{-0x1.e7f468p-1f,-0x1.e7f466p-1f}, 0x1.2c0d40p-1f}, //     1  
  {{-0x1.e7e276p-1f,-0x1.e7e274p-1f}, 0x1.2c1d3ep-1f}, //     1  
  {{-0x1.e7d078p-1f,-0x1.e7d078p-1f}, 0x1.2c2d40p-1f}, //     0  
  {{-0x1.e7be78p-1f,-0x1.e7be76p-1f}, 0x1.2c3d3ep-1f}, //     1  
  {{-0x1.e7ac70p-1f,-0x1.e7ac70p-1f}, 0x1.2c4d3cp-1f}, //     0  
  {{-0x1.e79a62p-1f,-0x1.e79a62p-1f}, 0x1.2c5d3ap-1f}, //     0  
  {{-0x1.e7884ap-1f,-0x1.e7884ap-1f}, 0x1.2c6d3ap-1f}, //     0  
  {{-0x1.e7762ep-1f,-0x1.e7762ep-1f}, 0x1.2c7d38p-1f}, //     0  
  {{-0x1.e76408p-1f,-0x1.e76408p-1f}, 0x1.2c8d38p-1f}, //     0  
  {{-0x1.e751dcp-1f,-0x1.e751dcp-1f}, 0x1.2c9d38p-1f}, //     0  
  {{-0x1.e73faap-1f,-0x1.e73faap-1f}, 0x1.2cad36p-1f}, //     0  
  {{-0x1.e72d72p-1f,-0x1.e72d72p-1f}, 0x1.2cbd34p-1f}, //     0  
  {{-0x1.e71b30p-1f,-0x1.e71b30p-1f}, 0x1.2ccd34p-1f}, //     0  
  {{-0x1.e708eap-1f,-0x1.e708eap-1f}, 0x1.2cdd32p-1f}, //     0  
  {{-0x1.e6f69ap-1f,-0x1.e6f69ap-1f}, 0x1.2ced32p-1f}, //     0  
  {{-0x1.e6e446p-1f,-0x1.e6e444p-1f}, 0x1.2cfd30p-1f}, //     1  
  {{-0x1.e6d1e8p-1f,-0x1.e6d1e6p-1f}, 0x1.2d0d30p-1f}, //     1  
  {{-0x1.e6bf84p-1f,-0x1.e6bf84p-1f}, 0x1.2d1d2ep-1f}, //     0  
  {{-0x1.e6ad18p-1f,-0x1.e6ad18p-1f}, 0x1.2d2d2ep-1f}, //     0  
  {{-0x1.e69aa8p-1f,-0x1.e69aa8p-1f}, 0x1.2d3d2cp-1f}, //     0  
  {{-0x1.e6882ep-1f,-0x1.e6882ep-1f}, 0x1.2d4d2cp-1f}, //     0  
  {{-0x1.e675b0p-1f,-0x1.e675b0p-1f}, 0x1.2d5d28p-1f}, //     0  
  {{-0x1.e66326p-1f,-0x1.e66326p-1f}, 0x1.2d6d2ap-1f}, //     0  
  {{-0x1.e6509ap-1f,-0x1.e65098p-1f}, 0x1.2d7d28p-1f}, //     1  
  {{-0x1.e63e02p-1f,-0x1.e63e02p-1f}, 0x1.2d8d28p-1f}, //     0  
  {{-0x1.e62b68p-1f,-0x1.e62b68p-1f}, 0x1.2d9d26p-1f}, //     0  
  {{-0x1.e618c6p-1f,-0x1.e618c4p-1f}, 0x1.2dad24p-1f}, //     1  
  {{-0x1.e6061ap-1f,-0x1.e6061ap-1f}, 0x1.2dbd24p-1f}, //     0  
  {{-0x1.e5f366p-1f,-0x1.e5f366p-1f}, 0x1.2dcd24p-1f}, //     0  
  {{-0x1.e5e0b0p-1f,-0x1.e5e0aep-1f}, 0x1.2ddd22p-1f}, //     1  
  {{-0x1.e5cdeep-1f,-0x1.e5cdeep-1f}, 0x1.2ded22p-1f}, //     0  
  {{-0x1.e5bb28p-1f,-0x1.e5bb28p-1f}, 0x1.2dfd20p-1f}, //     0  
  {{-0x1.e5a85ap-1f,-0x1.e5a85ap-1f}, 0x1.2e0d20p-1f}, //     0  
  {{-0x1.e59586p-1f,-0x1.e59586p-1f}, 0x1.2e1d1ep-1f}, //     0  
  {{-0x1.e582acp-1f,-0x1.e582aap-1f}, 0x1.2e2d1cp-1f}, //     1  
  {{-0x1.e56fc8p-1f,-0x1.e56fc6p-1f}, 0x1.2e3d1cp-1f}, //     1  
  {{-0x1.e55cdap-1f,-0x1.e55cd8p-1f}, 0x1.2e4d1ep-1f}, //     1  
  {{-0x1.e549eap-1f,-0x1.e549e8p-1f}, 0x1.2e5d1cp-1f}, //     1  
  {{-0x1.e536f2p-1f,-0x1.e536f2p-1f}, 0x1.2e6d1ap-1f}, //     0  
  {{-0x1.e523f4p-1f,-0x1.e523f4p-1f}, 0x1.2e7d18p-1f}, //     0  
  {{-0x1.e510ecp-1f,-0x1.e510ecp-1f}, 0x1.2e8d18p-1f}, //     0  
  {{-0x1.e4fde0p-1f,-0x1.e4fde0p-1f}, 0x1.2e9d16p-1f}, //     0  
  {{-0x1.e4eacap-1f,-0x1.e4eacap-1f}, 0x1.2ead16p-1f}, //     0  
  {{-0x1.e4d7b0p-1f,-0x1.e4d7b0p-1f}, 0x1.2ebd14p-1f}, //     0  
  {{-0x1.e4c48ap-1f,-0x1.e4c488p-1f}, 0x1.2ecd16p-1f}, //     1  
  {{-0x1.e4b162p-1f,-0x1.e4b162p-1f}, 0x1.2edd12p-1f}, //     0  
  {{-0x1.e49e30p-1f,-0x1.e49e30p-1f}, 0x1.2eed12p-1f}, //     0  
  {{-0x1.e48afap-1f,-0x1.e48afap-1f}, 0x1.2efd10p-1f}, //     0  
  {{-0x1.e477bap-1f,-0x1.e477b8p-1f}, 0x1.2f0d10p-1f}, //     1  
  {{-0x1.e46474p-1f,-0x1.e46474p-1f}, 0x1.2f1d0ep-1f}, //     0  
  {{-0x1.e45126p-1f,-0x1.e45124p-1f}, 0x1.2f2d0ep-1f}, //     1  
  {{-0x1.e43dd2p-1f,-0x1.e43dd2p-1f}, 0x1.2f3d0cp-1f}, //     0  
  {{-0x1.e42a76p-1f,-0x1.e42a74p-1f}, 0x1.2f4d0cp-1f}, //     1  
  {{-0x1.e41712p-1f,-0x1.e41710p-1f}, 0x1.2f5d0cp-1f}, //     1  
  {{-0x1.e403a8p-1f,-0x1.e403a8p-1f}, 0x1.2f6d0ap-1f}, //     0  
  {{-0x1.e3f038p-1f,-0x1.e3f038p-1f}, 0x1.2f7d08p-1f}, //     0  
  {{-0x1.e3dcc0p-1f,-0x1.e3dcc0p-1f}, 0x1.2f8d08p-1f}, //     0  
  {{-0x1.e3c942p-1f,-0x1.e3c942p-1f}, 0x1.2f9d06p-1f}, //     0  
  {{-0x1.e3b5b8p-1f,-0x1.e3b5b8p-1f}, 0x1.2fad08p-1f}, //     0  
  {{-0x1.e3a22cp-1f,-0x1.e3a22ap-1f}, 0x1.2fbd06p-1f}, //     1  
  {{-0x1.e38e98p-1f,-0x1.e38e98p-1f}, 0x1.2fcd04p-1f}, //     0  
  {{-0x1.e37afcp-1f,-0x1.e37afap-1f}, 0x1.2fdd04p-1f}, //     1  
  {{-0x1.e3675ap-1f,-0x1.e3675ap-1f}, 0x1.2fed02p-1f}, //     0  
  {{-0x1.e353b2p-1f,-0x1.e353b0p-1f}, 0x1.2ffd00p-1f}, //     1  
  {{-0x1.e34002p-1f,-0x1.e34000p-1f}, 0x1.300cfep-1f}, //     1  
  {{-0x1.e32c48p-1f,-0x1.e32c48p-1f}, 0x1.301cfep-1f}, //     0  
  {{-0x1.e31886p-1f,-0x1.e31884p-1f}, 0x1.302d00p-1f}, //     1  
  {{-0x1.e304c0p-1f,-0x1.e304c0p-1f}, 0x1.303cfep-1f}, //     0  
  {{-0x1.e2f0f4p-1f,-0x1.e2f0f4p-1f}, 0x1.304cfcp-1f}, //     0  
  {{-0x1.e2dd20p-1f,-0x1.e2dd20p-1f}, 0x1.305cfap-1f}, //     0  
  {{-0x1.e2c944p-1f,-0x1.e2c942p-1f}, 0x1.306cfap-1f}, //     1  
  {{-0x1.e2b562p-1f,-0x1.e2b562p-1f}, 0x1.307cf8p-1f}, //     0  
  {{-0x1.e2a176p-1f,-0x1.e2a176p-1f}, 0x1.308cf8p-1f}, //     0  
  {{-0x1.e28d86p-1f,-0x1.e28d86p-1f}, 0x1.309cf6p-1f}, //     0  
  {{-0x1.e2798cp-1f,-0x1.e2798cp-1f}, 0x1.30acf6p-1f}, //     0  
  {{-0x1.e26592p-1f,-0x1.e26590p-1f}, 0x1.30bcf2p-1f}, //     1  
  {{-0x1.e25186p-1f,-0x1.e25186p-1f}, 0x1.30ccf4p-1f}, //     0  
  {{-0x1.e23d7ap-1f,-0x1.e23d7ap-1f}, 0x1.30dcf2p-1f}, //     0  
  {{-0x1.e22964p-1f,-0x1.e22964p-1f}, 0x1.30ecf2p-1f}, //     0  
  {{-0x1.e2154ap-1f,-0x1.e2154ap-1f}, 0x1.30fcf0p-1f}, //     0  
  {{-0x1.e20126p-1f,-0x1.e20124p-1f}, 0x1.310cf0p-1f}, //     1  
  {{-0x1.e1ecfcp-1f,-0x1.e1ecfcp-1f}, 0x1.311ceep-1f}, //     0  
  {{-0x1.e1d8cap-1f,-0x1.e1d8cap-1f}, 0x1.312ceep-1f}, //     0  
  {{-0x1.e1c492p-1f,-0x1.e1c492p-1f}, 0x1.313cecp-1f}, //     0  
  {{-0x1.e1b052p-1f,-0x1.e1b050p-1f}, 0x1.314cecp-1f}, //     1  
  {{-0x1.e19c0ap-1f,-0x1.e19c08p-1f}, 0x1.315cecp-1f}, //     1  
  {{-0x1.e187bcp-1f,-0x1.e187bcp-1f}, 0x1.316ceap-1f}, //     0  
  {{-0x1.e17368p-1f,-0x1.e17368p-1f}, 0x1.317ce8p-1f}, //     0  
  {{-0x1.e15f0ap-1f,-0x1.e15f0ap-1f}, 0x1.318ce8p-1f}, //     0  
  {{-0x1.e14aa8p-1f,-0x1.e14aa8p-1f}, 0x1.319ce6p-1f}, //     0  
  {{-0x1.e13640p-1f,-0x1.e1363ep-1f}, 0x1.31ace4p-1f}, //     1  
  {{-0x1.e121ccp-1f,-0x1.e121ccp-1f}, 0x1.31bce4p-1f}, //     0  
  {{-0x1.e10d50p-1f,-0x1.e10d4ep-1f}, 0x1.31cce6p-1f}, //     1  
  {{-0x1.e0f8d2p-1f,-0x1.e0f8d2p-1f}, 0x1.31dce2p-1f}, //     0  
  {{-0x1.e0e44ap-1f,-0x1.e0e44ap-1f}, 0x1.31ece2p-1f}, //     0  
  {{-0x1.e0cfbep-1f,-0x1.e0cfbcp-1f}, 0x1.31fce0p-1f}, //     1  
  {{-0x1.e0bb26p-1f,-0x1.e0bb26p-1f}, 0x1.320ce0p-1f}, //     0  
  {{-0x1.e0a68ap-1f,-0x1.e0a68ap-1f}, 0x1.321cdep-1f}, //     0  
  {{-0x1.e091e6p-1f,-0x1.e091e4p-1f}, 0x1.322cdep-1f}, //     1  
  {{-0x1.e07d3cp-1f,-0x1.e07d3cp-1f}, 0x1.323cdcp-1f}, //     0  
  {{-0x1.e06886p-1f,-0x1.e06884p-1f}, 0x1.324cdep-1f}, //     1  
  {{-0x1.e053d0p-1f,-0x1.e053d0p-1f}, 0x1.325cdap-1f}, //     0  
  {{-0x1.e03f0cp-1f,-0x1.e03f0ap-1f}, 0x1.326cdcp-1f}, //     1  
  {{-0x1.e02a48p-1f,-0x1.e02a46p-1f}, 0x1.327cd8p-1f}, //     1  
  {{-0x1.e01576p-1f,-0x1.e01576p-1f}, 0x1.328cd8p-1f}, //     0  
  {{-0x1.e000a2p-1f,-0x1.e000a2p-1f}, 0x1.329cd6p-1f}, //     0  
  {{-0x1.dfebc4p-1f,-0x1.dfebc2p-1f}, 0x1.32acd6p-1f}, //     1  
  {{-0x1.dfd6e0p-1f,-0x1.dfd6e0p-1f}, 0x1.32bcd4p-1f}, //     0  
  {{-0x1.dfc1f6p-1f,-0x1.dfc1f4p-1f}, 0x1.32ccd2p-1f}, //     1  
  {{-0x1.dfad02p-1f,-0x1.dfad00p-1f}, 0x1.32dcd2p-1f}, //     1  
  {{-0x1.df9804p-1f,-0x1.df9802p-1f}, 0x1.32ecd4p-1f}, //     1  
  {{-0x1.df8306p-1f,-0x1.df8306p-1f}, 0x1.32fcd0p-1f}, //     0  
  {{-0x1.df6dfap-1f,-0x1.df6df8p-1f}, 0x1.330cd2p-1f}, //     1  
  {{-0x1.df58eep-1f,-0x1.df58ecp-1f}, 0x1.331ccep-1f}, //     1  
  {{-0x1.df43d6p-1f,-0x1.df43d4p-1f}, 0x1.332ccep-1f}, //     1  
  {{-0x1.df2eb8p-1f,-0x1.df2eb8p-1f}, 0x1.333cccp-1f}, //     0  
  {{-0x1.df1992p-1f,-0x1.df1992p-1f}, 0x1.334cccp-1f}, //     0  
  {{-0x1.df0466p-1f,-0x1.df0466p-1f}, 0x1.335ccap-1f}, //     0  
  {{-0x1.deef32p-1f,-0x1.deef30p-1f}, 0x1.336ccap-1f}, //     1  
  {{-0x1.ded9f8p-1f,-0x1.ded9f8p-1f}, 0x1.337cc8p-1f}, //     0  
  {{-0x1.dec4b4p-1f,-0x1.dec4b4p-1f}, 0x1.338cc8p-1f}, //     0  
  {{-0x1.deaf70p-1f,-0x1.deaf6ep-1f}, 0x1.339cc4p-1f}, //     1  
  {{-0x1.de9a1ap-1f,-0x1.de9a1ap-1f}, 0x1.33acc6p-1f}, //     0  
  {{-0x1.de84c4p-1f,-0x1.de84c4p-1f}, 0x1.33bcc4p-1f}, //     0  
  {{-0x1.de6f64p-1f,-0x1.de6f64p-1f}, 0x1.33ccc4p-1f}, //     0  
  {{-0x1.de5a00p-1f,-0x1.de59fep-1f}, 0x1.33dcc2p-1f}, //     1  
  {{-0x1.de4490p-1f,-0x1.de4490p-1f}, 0x1.33ecc2p-1f}, //     0  
  {{-0x1.de2f1ep-1f,-0x1.de2f1cp-1f}, 0x1.33fcc0p-1f}, //     1  
  {{-0x1.de19a0p-1f,-0x1.de19a0p-1f}, 0x1.340cc0p-1f}, //     0  
  {{-0x1.de041ep-1f,-0x1.de041ep-1f}, 0x1.341cbep-1f}, //     0  
  {{-0x1.ddee92p-1f,-0x1.ddee92p-1f}, 0x1.342cbep-1f}, //     0  
  {{-0x1.ddd906p-1f,-0x1.ddd904p-1f}, 0x1.343cbap-1f}, //     1  
  {{-0x1.ddc368p-1f,-0x1.ddc368p-1f}, 0x1.344cbcp-1f}, //     0  
  {{-0x1.ddadc8p-1f,-0x1.ddadc6p-1f}, 0x1.345cbcp-1f}, //     1  
  {{-0x1.dd9822p-1f,-0x1.dd9820p-1f}, 0x1.346cbap-1f}, //     1  
  {{-0x1.dd8274p-1f,-0x1.dd8274p-1f}, 0x1.347cb8p-1f}, //     0  
  {{-0x1.dd6cbep-1f,-0x1.dd6cbep-1f}, 0x1.348cb8p-1f}, //     0  
  {{-0x1.dd5702p-1f,-0x1.dd5702p-1f}, 0x1.349cb6p-1f}, //     0  
  {{-0x1.dd4140p-1f,-0x1.dd413ep-1f}, 0x1.34acb4p-1f}, //     1  
  {{-0x1.dd2b74p-1f,-0x1.dd2b72p-1f}, 0x1.34bcb4p-1f}, //     1  
  {{-0x1.dd15a0p-1f,-0x1.dd159ep-1f}, 0x1.34ccb4p-1f}, //     1  
  {{-0x1.dcffcap-1f,-0x1.dcffc8p-1f}, 0x1.34dcb0p-1f}, //     1  
  {{-0x1.dce9e4p-1f,-0x1.dce9e4p-1f}, 0x1.34ecb2p-1f}, //     0  
  {{-0x1.dcd3fep-1f,-0x1.dcd3fcp-1f}, 0x1.34fcb0p-1f}, //     1  
  {{-0x1.dcbe0cp-1f,-0x1.dcbe0cp-1f}, 0x1.350cb0p-1f}, //     0  
  {{-0x1.dca818p-1f,-0x1.dca816p-1f}, 0x1.351caep-1f}, //     1  
  {{-0x1.dc9218p-1f,-0x1.dc9218p-1f}, 0x1.352caep-1f}, //     0  
  {{-0x1.dc7c14p-1f,-0x1.dc7c14p-1f}, 0x1.353cacp-1f}, //     0  
  {{-0x1.dc6606p-1f,-0x1.dc6606p-1f}, 0x1.354cacp-1f}, //     0  
  {{-0x1.dc4ff4p-1f,-0x1.dc4ff4p-1f}, 0x1.355caap-1f}, //     0  
  {{-0x1.dc39d8p-1f,-0x1.dc39d8p-1f}, 0x1.356caap-1f}, //     0  
  {{-0x1.dc23b8p-1f,-0x1.dc23b6p-1f}, 0x1.357ca8p-1f}, //     1  
  {{-0x1.dc0d90p-1f,-0x1.dc0d8ep-1f}, 0x1.358ca6p-1f}, //     1  
  {{-0x1.dbf75ep-1f,-0x1.dbf75cp-1f}, 0x1.359ca6p-1f}, //     1  
  {{-0x1.dbe122p-1f,-0x1.dbe120p-1f}, 0x1.35aca8p-1f}, //     1  
  {{-0x1.dbcae6p-1f,-0x1.dbcae6p-1f}, 0x1.35bca4p-1f}, //     0  
  {{-0x1.dbb49ep-1f,-0x1.dbb49ep-1f}, 0x1.35cca4p-1f}, //     0  
  {{-0x1.db9e50p-1f,-0x1.db9e4ep-1f}, 0x1.35dca4p-1f}, //     1  
  {{-0x1.db87fcp-1f,-0x1.db87fcp-1f}, 0x1.35eca2p-1f}, //     0  
  {{-0x1.db71a2p-1f,-0x1.db71a0p-1f}, 0x1.35fca0p-1f}, //     1  
  {{-0x1.db5b3cp-1f,-0x1.db5b3cp-1f}, 0x1.360ca0p-1f}, //     0  
  {{-0x1.db44d2p-1f,-0x1.db44d2p-1f}, 0x1.361c9ep-1f}, //     0  
  {{-0x1.db2e60p-1f,-0x1.db2e5ep-1f}, 0x1.362c9ep-1f}, //     1  
  {{-0x1.db17e8p-1f,-0x1.db17e6p-1f}, 0x1.363c9cp-1f}, //     1  
  {{-0x1.db0166p-1f,-0x1.db0164p-1f}, 0x1.364c9cp-1f}, //     1  
  {{-0x1.daeae0p-1f,-0x1.daeadep-1f}, 0x1.365c9ap-1f}, //     1  
  {{-0x1.dad44ep-1f,-0x1.dad44ep-1f}, 0x1.366c9ap-1f}, //     0  
  {{-0x1.dabdbap-1f,-0x1.dabdbap-1f}, 0x1.367c98p-1f}, //     0  
  {{-0x1.daa71cp-1f,-0x1.daa71ap-1f}, 0x1.368c98p-1f}, //     1  
  {{-0x1.da9078p-1f,-0x1.da9076p-1f}, 0x1.369c96p-1f}, //     1  
  {{-0x1.da79cap-1f,-0x1.da79cap-1f}, 0x1.36ac96p-1f}, //     0  
  {{-0x1.da6316p-1f,-0x1.da6314p-1f}, 0x1.36bc96p-1f}, //     1  
  {{-0x1.da4c5cp-1f,-0x1.da4c5cp-1f}, 0x1.36cc94p-1f}, //     0  
  {{-0x1.da359cp-1f,-0x1.da359ap-1f}, 0x1.36dc92p-1f}, //     1  
  {{-0x1.da1ed0p-1f,-0x1.da1ed0p-1f}, 0x1.36ec92p-1f}, //     0  
  {{-0x1.da0802p-1f,-0x1.da0800p-1f}, 0x1.36fc90p-1f}, //     1  
  {{-0x1.d9f128p-1f,-0x1.d9f128p-1f}, 0x1.370c90p-1f}, //     0  
  {{-0x1.d9da4ap-1f,-0x1.d9da4ap-1f}, 0x1.371c8ep-1f}, //     0  
  {{-0x1.d9c366p-1f,-0x1.d9c364p-1f}, 0x1.372c8cp-1f}, //     1  
  {{-0x1.d9ac76p-1f,-0x1.d9ac76p-1f}, 0x1.373c8cp-1f}, //     0  
  {{-0x1.d9957ep-1f,-0x1.d9957cp-1f}, 0x1.374c8ep-1f}, //     1  
  {{-0x1.d97e86p-1f,-0x1.d97e84p-1f}, 0x1.375c8ap-1f}, //     1  
  {{-0x1.d9677ep-1f,-0x1.d9677cp-1f}, 0x1.376c8cp-1f}, //     1  
  {{-0x1.d95076p-1f,-0x1.d95076p-1f}, 0x1.377c88p-1f}, //     0  
  {{-0x1.d93964p-1f,-0x1.d93962p-1f}, 0x1.378c88p-1f}, //     1  
  {{-0x1.d9224cp-1f,-0x1.d9224ap-1f}, 0x1.379c86p-1f}, //     1  
  {{-0x1.d90b28p-1f,-0x1.d90b28p-1f}, 0x1.37ac86p-1f}, //     0  
  {{-0x1.d8f402p-1f,-0x1.d8f402p-1f}, 0x1.37bc84p-1f}, //     0  
  {{-0x1.d8dcd2p-1f,-0x1.d8dcd0p-1f}, 0x1.37cc84p-1f}, //     1  
  {{-0x1.d8c59cp-1f,-0x1.d8c59cp-1f}, 0x1.37dc82p-1f}, //     0  
  {{-0x1.d8ae5cp-1f,-0x1.d8ae5cp-1f}, 0x1.37ec82p-1f}, //     0  
  {{-0x1.d89718p-1f,-0x1.d89718p-1f}, 0x1.37fc80p-1f}, //     0  
  {{-0x1.d87fcep-1f,-0x1.d87fccp-1f}, 0x1.380c7ep-1f}, //     1  
  {{-0x1.d86878p-1f,-0x1.d86878p-1f}, 0x1.381c7ep-1f}, //     0  
  {{-0x1.d8511cp-1f,-0x1.d8511ap-1f}, 0x1.382c7ep-1f}, //     1  
  {{-0x1.d839bap-1f,-0x1.d839bap-1f}, 0x1.383c7cp-1f}, //     0  
  {{-0x1.d82250p-1f,-0x1.d8224ep-1f}, 0x1.384c7cp-1f}, //     1  
  {{-0x1.d80ae0p-1f,-0x1.d80adep-1f}, 0x1.385c7ap-1f}, //     1  
  {{-0x1.d7f366p-1f,-0x1.d7f364p-1f}, 0x1.386c7ap-1f}, //     1  
  {{-0x1.d7dbe8p-1f,-0x1.d7dbe6p-1f}, 0x1.387c78p-1f}, //     1  
  {{-0x1.d7c45ep-1f,-0x1.d7c45ep-1f}, 0x1.388c78p-1f}, //     0  
  {{-0x1.d7acd2p-1f,-0x1.d7acd0p-1f}, 0x1.389c76p-1f}, //     1  
  {{-0x1.d7953ap-1f,-0x1.d7953ap-1f}, 0x1.38ac76p-1f}, //     0  
  {{-0x1.d77d9ep-1f,-0x1.d77d9ep-1f}, 0x1.38bc74p-1f}, //     0  
  {{-0x1.d765fap-1f,-0x1.d765f8p-1f}, 0x1.38cc74p-1f}, //     1  
  {{-0x1.d74e4ep-1f,-0x1.d74e4ep-1f}, 0x1.38dc72p-1f}, //     0  
  {{-0x1.d7369ap-1f,-0x1.d7369ap-1f}, 0x1.38ec72p-1f}, //     0  
  {{-0x1.d71ee2p-1f,-0x1.d71ee0p-1f}, 0x1.38fc70p-1f}, //     1  
  {{-0x1.d7071ep-1f,-0x1.d7071ep-1f}, 0x1.390c70p-1f}, //     0  
  {{-0x1.d6ef56p-1f,-0x1.d6ef56p-1f}, 0x1.391c6ep-1f}, //     0  
  {{-0x1.d6d784p-1f,-0x1.d6d784p-1f}, 0x1.392c6ep-1f}, //     0  
  {{-0x1.d6bfaep-1f,-0x1.d6bfaep-1f}, 0x1.393c6cp-1f}, //     0  
  {{-0x1.d6a7cep-1f,-0x1.d6a7ccp-1f}, 0x1.394c6cp-1f}, //     1  
  {{-0x1.d68fecp-1f,-0x1.d68feap-1f}, 0x1.395c68p-1f}, //     1  
  {{-0x1.d677fap-1f,-0x1.d677f8p-1f}, 0x1.396c6ap-1f}, //     1  
  {{-0x1.d66006p-1f,-0x1.d66006p-1f}, 0x1.397c68p-1f}, //     0  
  {{-0x1.d64806p-1f,-0x1.d64804p-1f}, 0x1.398c6ap-1f}, //     1  
  {{-0x1.d63006p-1f,-0x1.d63004p-1f}, 0x1.399c66p-1f}, //     1  
  {{-0x1.d617fap-1f,-0x1.d617f8p-1f}, 0x1.39ac66p-1f}, //     1  
  {{-0x1.d5ffe6p-1f,-0x1.d5ffe4p-1f}, 0x1.39bc66p-1f}, //     1  
  {{-0x1.d5e7ccp-1f,-0x1.d5e7ccp-1f}, 0x1.39cc64p-1f}, //     0  
  {{-0x1.d5cfaep-1f,-0x1.d5cfacp-1f}, 0x1.39dc62p-1f}, //     1  
  {{-0x1.d5b784p-1f,-0x1.d5b782p-1f}, 0x1.39ec62p-1f}, //     1  
  {{-0x1.d59f54p-1f,-0x1.d59f54p-1f}, 0x1.39fc60p-1f}, //     0  
  {{-0x1.d5871cp-1f,-0x1.d5871ap-1f}, 0x1.3a0c60p-1f}, //     1  
  {{-0x1.d56edep-1f,-0x1.d56edep-1f}, 0x1.3a1c5ep-1f}, //     0  
  {{-0x1.d55698p-1f,-0x1.d55696p-1f}, 0x1.3a2c5ep-1f}, //     1  
  {{-0x1.d53e4cp-1f,-0x1.d53e4ap-1f}, 0x1.3a3c5cp-1f}, //     1  
  {{-0x1.d525f6p-1f,-0x1.d525f4p-1f}, 0x1.3a4c5cp-1f}, //     1  
  {{-0x1.d50d9ap-1f,-0x1.d50d9ap-1f}, 0x1.3a5c5ap-1f}, //     0  
  {{-0x1.d4f536p-1f,-0x1.d4f534p-1f}, 0x1.3a6c5ap-1f}, //     1  
  {{-0x1.d4dcccp-1f,-0x1.d4dcccp-1f}, 0x1.3a7c58p-1f}, //     0  
  {{-0x1.d4c45ap-1f,-0x1.d4c458p-1f}, 0x1.3a8c58p-1f}, //     1  
  {{-0x1.d4abe2p-1f,-0x1.d4abe0p-1f}, 0x1.3a9c56p-1f}, //     1  
  {{-0x1.d49362p-1f,-0x1.d49360p-1f}, 0x1.3aac54p-1f}, //     1  
  {{-0x1.d47ad8p-1f,-0x1.d47ad6p-1f}, 0x1.3abc54p-1f}, //     1  
  {{-0x1.d46246p-1f,-0x1.d46246p-1f}, 0x1.3acc54p-1f}, //     0  
  {{-0x1.d449b2p-1f,-0x1.d449b0p-1f}, 0x1.3adc52p-1f}, //     1  
  {{-0x1.d43112p-1f,-0x1.d43110p-1f}, 0x1.3aec52p-1f}, //     1  
  {{-0x1.d4186ep-1f,-0x1.d4186cp-1f}, 0x1.3afc50p-1f}, //     1  
  {{-0x1.d3ffbep-1f,-0x1.d3ffbep-1f}, 0x1.3b0c50p-1f}, //     0  
  {{-0x1.d3e70cp-1f,-0x1.d3e70ap-1f}, 0x1.3b1c4ep-1f}, //     1  
  {{-0x1.d3ce4ep-1f,-0x1.d3ce4cp-1f}, 0x1.3b2c4ep-1f}, //     1  
  {{-0x1.d3b58ap-1f,-0x1.d3b588p-1f}, 0x1.3b3c4ep-1f}, //     1  
  {{-0x1.d39cc0p-1f,-0x1.d39cc0p-1f}, 0x1.3b4c4cp-1f}, //     0  
  {{-0x1.d383f0p-1f,-0x1.d383eep-1f}, 0x1.3b5c4ap-1f}, //     1  
  {{-0x1.d36b14p-1f,-0x1.d36b14p-1f}, 0x1.3b6c4ap-1f}, //     0  
  {{-0x1.d35236p-1f,-0x1.d35234p-1f}, 0x1.3b7c48p-1f}, //     1  
  {{-0x1.d3394cp-1f,-0x1.d3394ap-1f}, 0x1.3b8c48p-1f}, //     1  
  {{-0x1.d3205ep-1f,-0x1.d3205cp-1f}, 0x1.3b9c46p-1f}, //     1  
  {{-0x1.d30764p-1f,-0x1.d30764p-1f}, 0x1.3bac46p-1f}, //     0  
  {{-0x1.d2ee68p-1f,-0x1.d2ee68p-1f}, 0x1.3bbc44p-1f}, //     0  
  {{-0x1.d2d560p-1f,-0x1.d2d560p-1f}, 0x1.3bcc44p-1f}, //     0  
  {{-0x1.d2bc56p-1f,-0x1.d2bc54p-1f}, 0x1.3bdc42p-1f}, //     1  
  {{-0x1.d2a33ep-1f,-0x1.d2a33ep-1f}, 0x1.3bec42p-1f}, //     0  
  {{-0x1.d28a24p-1f,-0x1.d28a24p-1f}, 0x1.3bfc40p-1f}, //     0  
  {{-0x1.d27100p-1f,-0x1.d270fep-1f}, 0x1.3c0c40p-1f}, //     1  
  {{-0x1.d257d6p-1f,-0x1.d257d6p-1f}, 0x1.3c1c3ep-1f}, //     0  
  {{-0x1.d23ea2p-1f,-0x1.d23ea2p-1f}, 0x1.3c2c3ep-1f}, //     0  
  {{-0x1.d2256ap-1f,-0x1.d2256ap-1f}, 0x1.3c3c3cp-1f}, //     0  
  {{-0x1.d20c28p-1f,-0x1.d20c26p-1f}, 0x1.3c4c3cp-1f}, //     1  
  {{-0x1.d1f2e2p-1f,-0x1.d1f2e0p-1f}, 0x1.3c5c3ap-1f}, //     1  
  {{-0x1.d1d990p-1f,-0x1.d1d98ep-1f}, 0x1.3c6c3ap-1f}, //     1  
  {{-0x1.d1c03ap-1f,-0x1.d1c038p-1f}, 0x1.3c7c38p-1f}, //     1  
  {{-0x1.d1a6dap-1f,-0x1.d1a6d8p-1f}, 0x1.3c8c38p-1f}, //     1  
  {{-0x1.d18d76p-1f,-0x1.d18d74p-1f}, 0x1.3c9c36p-1f}, //     1  
  {{-0x1.d17404p-1f,-0x1.d17402p-1f}, 0x1.3cac38p-1f}, //     1  
  {{-0x1.d15a96p-1f,-0x1.d15a94p-1f}, 0x1.3cbc32p-1f}, //     1  
  {{-0x1.d14114p-1f,-0x1.d14114p-1f}, 0x1.3ccc34p-1f}, //     0  
  {{-0x1.d12796p-1f,-0x1.d12794p-1f}, 0x1.3cdc30p-1f}, //     1  
  {{-0x1.d10e06p-1f,-0x1.d10e04p-1f}, 0x1.3cec32p-1f}, //     1  
  {{-0x1.d0f476p-1f,-0x1.d0f474p-1f}, 0x1.3cfc30p-1f}, //     1  
  {{-0x1.d0dadap-1f,-0x1.d0dad8p-1f}, 0x1.3d0c30p-1f}, //     1  
  {{-0x1.d0c13ap-1f,-0x1.d0c138p-1f}, 0x1.3d1c2ep-1f}, //     1  
  {{-0x1.d0a790p-1f,-0x1.d0a78ep-1f}, 0x1.3d2c2ep-1f}, //     1  
  {{-0x1.d08ddep-1f,-0x1.d08ddcp-1f}, 0x1.3d3c2ep-1f}, //     1  
  {{-0x1.d07426p-1f,-0x1.d07426p-1f}, 0x1.3d4c2cp-1f}, //     0  
  {{-0x1.d05a6ap-1f,-0x1.d05a68p-1f}, 0x1.3d5c2ap-1f}, //     1  
  {{-0x1.d040a2p-1f,-0x1.d040a0p-1f}, 0x1.3d6c2ap-1f}, //     1  
  {{-0x1.d026d4p-1f,-0x1.d026d4p-1f}, 0x1.3d7c28p-1f}, //     0  
  {{-0x1.d00cfep-1f,-0x1.d00cfcp-1f}, 0x1.3d8c28p-1f}, //     1  
  {{-0x1.cff320p-1f,-0x1.cff31ep-1f}, 0x1.3d9c28p-1f}, //     1  
  {{-0x1.cfd93cp-1f,-0x1.cfd93cp-1f}, 0x1.3dac26p-1f}, //     0  
  {{-0x1.cfbf52p-1f,-0x1.cfbf50p-1f}, 0x1.3dbc24p-1f}, //     1  
  {{-0x1.cfa55ep-1f,-0x1.cfa55cp-1f}, 0x1.3dcc24p-1f}, //     1  
  {{-0x1.cf8b64p-1f,-0x1.cf8b62p-1f}, 0x1.3ddc22p-1f}, //     1  
  {{-0x1.cf7160p-1f,-0x1.cf7160p-1f}, 0x1.3dec22p-1f}, //     0  
  {{-0x1.cf5758p-1f,-0x1.cf5758p-1f}, 0x1.3dfc20p-1f}, //     0  
  {{-0x1.cf3d46p-1f,-0x1.cf3d44p-1f}, 0x1.3e0c20p-1f}, //     1  
  {{-0x1.cf232ep-1f,-0x1.cf232ep-1f}, 0x1.3e1c1ep-1f}, //     0  
  {{-0x1.cf090ep-1f,-0x1.cf090cp-1f}, 0x1.3e2c1ep-1f}, //     1  
  {{-0x1.ceeee8p-1f,-0x1.ceeee6p-1f}, 0x1.3e3c1cp-1f}, //     1  
  {{-0x1.ced4b8p-1f,-0x1.ced4b6p-1f}, 0x1.3e4c1cp-1f}, //     1  
  {{-0x1.ceba82p-1f,-0x1.ceba82p-1f}, 0x1.3e5c1ap-1f}, //     0  
  {{-0x1.cea044p-1f,-0x1.cea042p-1f}, 0x1.3e6c1ap-1f}, //     1  
  {{-0x1.ce8600p-1f,-0x1.ce85fep-1f}, 0x1.3e7c18p-1f}, //     1  
  {{-0x1.ce6bb2p-1f,-0x1.ce6bb0p-1f}, 0x1.3e8c18p-1f}, //     1  
  {{-0x1.ce515ep-1f,-0x1.ce515ep-1f}, 0x1.3e9c16p-1f}, //     0  
  {{-0x1.ce3702p-1f,-0x1.ce3700p-1f}, 0x1.3eac16p-1f}, //     1  
  {{-0x1.ce1ca0p-1f,-0x1.ce1c9ep-1f}, 0x1.3ebc14p-1f}, //     1  
  {{-0x1.ce0234p-1f,-0x1.ce0232p-1f}, 0x1.3ecc14p-1f}, //     1  
  {{-0x1.cde7c4p-1f,-0x1.cde7c2p-1f}, 0x1.3edc12p-1f}, //     1  
  {{-0x1.cdcd48p-1f,-0x1.cdcd46p-1f}, 0x1.3eec12p-1f}, //     1  
  {{-0x1.cdb2c8p-1f,-0x1.cdb2c8p-1f}, 0x1.3efc10p-1f}, //     0  
  {{-0x1.cd983ep-1f,-0x1.cd983ep-1f}, 0x1.3f0c10p-1f}, //     0  
  {{-0x1.cd7db0p-1f,-0x1.cd7db0p-1f}, 0x1.3f1c0ep-1f}, //     0  
  {{-0x1.cd6318p-1f,-0x1.cd6316p-1f}, 0x1.3f2c0ep-1f}, //     1  
  {{-0x1.cd487ap-1f,-0x1.cd487ap-1f}, 0x1.3f3c0cp-1f}, //     0  
  {{-0x1.cd2dd2p-1f,-0x1.cd2dd2p-1f}, 0x1.3f4c0cp-1f}, //     0  
  {{-0x1.cd1326p-1f,-0x1.cd1324p-1f}, 0x1.3f5c0ap-1f}, //     1  
  {{-0x1.ccf870p-1f,-0x1.ccf86ep-1f}, 0x1.3f6c0ap-1f}, //     1  
  {{-0x1.ccddb4p-1f,-0x1.ccddb2p-1f}, 0x1.3f7c08p-1f}, //     1  
  {{-0x1.ccc2eep-1f,-0x1.ccc2ecp-1f}, 0x1.3f8c08p-1f}, //     1  
  {{-0x1.cca824p-1f,-0x1.cca822p-1f}, 0x1.3f9c06p-1f}, //     1  
  {{-0x1.cc8d50p-1f,-0x1.cc8d4ep-1f}, 0x1.3fac06p-1f}, //     1  
  {{-0x1.cc7276p-1f,-0x1.cc7274p-1f}, 0x1.3fbc04p-1f}, //     1  
  {{-0x1.cc5792p-1f,-0x1.cc5790p-1f}, 0x1.3fcc04p-1f}, //     1  
  {{-0x1.cc3caap-1f,-0x1.cc3ca8p-1f}, 0x1.3fdc02p-1f}, //     1  
  {{-0x1.cc21b8p-1f,-0x1.cc21b6p-1f}, 0x1.3fec02p-1f}, //     1  
  {{-0x1.cc06c0p-1f,-0x1.cc06c0p-1f}, 0x1.3ffc00p-1f}, //     0  
  {{-0x1.cbebbep-1f,-0x1.cbebbep-1f}, 0x1.400c00p-1f}, //     0  
  {{-0x1.cbd0b8p-1f,-0x1.cbd0b8p-1f}, 0x1.401bfep-1f}, //     0  
  {{-0x1.cbb5a8p-1f,-0x1.cbb5a6p-1f}, 0x1.402bfep-1f}, //     1  
  {{-0x1.cb9a92p-1f,-0x1.cb9a92p-1f}, 0x1.403bfcp-1f}, //     0  
  {{-0x1.cb7f72p-1f,-0x1.cb7f72p-1f}, 0x1.404bfcp-1f}, //     0  
  {{-0x1.cb644ep-1f,-0x1.cb644ep-1f}, 0x1.405bfap-1f}, //     0  
  {{-0x1.cb4920p-1f,-0x1.cb491ep-1f}, 0x1.406bfap-1f}, //     1  
  {{-0x1.cb2decp-1f,-0x1.cb2decp-1f}, 0x1.407bf8p-1f}, //     0  
  {{-0x1.cb12b0p-1f,-0x1.cb12aep-1f}, 0x1.408bf8p-1f}, //     1  
  {{-0x1.caf76ep-1f,-0x1.caf76cp-1f}, 0x1.409bf6p-1f}, //     1  
  {{-0x1.cadc20p-1f,-0x1.cadc1ep-1f}, 0x1.40abf6p-1f}, //     1  
  {{-0x1.cac0d0p-1f,-0x1.cac0cep-1f}, 0x1.40bbf4p-1f}, //     1  
  {{-0x1.caa574p-1f,-0x1.caa572p-1f}, 0x1.40cbf4p-1f}, //     1  
  {{-0x1.ca8a14p-1f,-0x1.ca8a12p-1f}, 0x1.40dbf2p-1f}, //     1  
  {{-0x1.ca6ea8p-1f,-0x1.ca6ea6p-1f}, 0x1.40ebf2p-1f}, //     1  
  {{-0x1.ca5338p-1f,-0x1.ca5338p-1f}, 0x1.40fbf0p-1f}, //     0  
  {{-0x1.ca37bep-1f,-0x1.ca37bep-1f}, 0x1.410bf0p-1f}, //     0  
  {{-0x1.ca1c40p-1f,-0x1.ca1c40p-1f}, 0x1.411beep-1f}, //     0  
  {{-0x1.ca00b8p-1f,-0x1.ca00b6p-1f}, 0x1.412beep-1f}, //     1  
  {{-0x1.c9e52ap-1f,-0x1.c9e52ap-1f}, 0x1.413becp-1f}, //     0  
  {{-0x1.c9c992p-1f,-0x1.c9c990p-1f}, 0x1.414becp-1f}, //     1  
  {{-0x1.c9adf6p-1f,-0x1.c9adf4p-1f}, 0x1.415beap-1f}, //     1  
  {{-0x1.c99250p-1f,-0x1.c9924ep-1f}, 0x1.416beap-1f}, //     1  
  {{-0x1.c976a4p-1f,-0x1.c976a2p-1f}, 0x1.417be8p-1f}, //     1  
  {{-0x1.c95aeep-1f,-0x1.c95aecp-1f}, 0x1.418be8p-1f}, //     1  
  {{-0x1.c93f34p-1f,-0x1.c93f32p-1f}, 0x1.419be6p-1f}, //     1  
  {{-0x1.c9236ep-1f,-0x1.c9236cp-1f}, 0x1.41abe6p-1f}, //     1  
  {{-0x1.c907a4p-1f,-0x1.c907a2p-1f}, 0x1.41bbe4p-1f}, //     1  
  {{-0x1.c8ebd0p-1f,-0x1.c8ebcep-1f}, 0x1.41cbe4p-1f}, //     1  
  {{-0x1.c8cff8p-1f,-0x1.c8cff6p-1f}, 0x1.41dbe2p-1f}, //     1  
  {{-0x1.c8b414p-1f,-0x1.c8b412p-1f}, 0x1.41ebe2p-1f}, //     1  
  {{-0x1.c8982cp-1f,-0x1.c8982cp-1f}, 0x1.41fbe0p-1f}, //     0  
  {{-0x1.c87c3ap-1f,-0x1.c87c38p-1f}, 0x1.420be0p-1f}, //     1  
  {{-0x1.c86044p-1f,-0x1.c86042p-1f}, 0x1.421bdep-1f}, //     1  
  {{-0x1.c84442p-1f,-0x1.c84440p-1f}, 0x1.422bdep-1f}, //     1  
  {{-0x1.c8283cp-1f,-0x1.c8283ap-1f}, 0x1.423bdcp-1f}, //     1  
  {{-0x1.c80c2cp-1f,-0x1.c80c2ap-1f}, 0x1.424bdcp-1f}, //     1  
  {{-0x1.c7f016p-1f,-0x1.c7f014p-1f}, 0x1.425bdap-1f}, //     1  
  {{-0x1.c7d3f6p-1f,-0x1.c7d3f6p-1f}, 0x1.426bdap-1f}, //     0  
  {{-0x1.c7b7d0p-1f,-0x1.c7b7cep-1f}, 0x1.427bdap-1f}, //     1  
  {{-0x1.c79ba4p-1f,-0x1.c79ba2p-1f}, 0x1.428bd8p-1f}, //     1  
  {{-0x1.c77f6ep-1f,-0x1.c77f6cp-1f}, 0x1.429bd8p-1f}, //     1  
  {{-0x1.c76332p-1f,-0x1.c76330p-1f}, 0x1.42abd6p-1f}, //     1  
  {{-0x1.c746f0p-1f,-0x1.c746eep-1f}, 0x1.42bbd4p-1f}, //     1  
  {{-0x1.c72aa4p-1f,-0x1.c72aa2p-1f}, 0x1.42cbd4p-1f}, //     1  
  {{-0x1.c70e52p-1f,-0x1.c70e50p-1f}, 0x1.42dbd2p-1f}, //     1  
  {{-0x1.c6f1f6p-1f,-0x1.c6f1f4p-1f}, 0x1.42ebd2p-1f}, //     1  
  {{-0x1.c6d596p-1f,-0x1.c6d594p-1f}, 0x1.42fbd0p-1f}, //     1  
  {{-0x1.c6b92ap-1f,-0x1.c6b928p-1f}, 0x1.430bd0p-1f}, //     1  
  {{-0x1.c69cbap-1f,-0x1.c69cb8p-1f}, 0x1.431bcep-1f}, //     1  
  {{-0x1.c68040p-1f,-0x1.c6803ep-1f}, 0x1.432bcep-1f}, //     1  
  {{-0x1.c663c4p-1f,-0x1.c663c2p-1f}, 0x1.433bcap-1f}, //     1  
  {{-0x1.c64738p-1f,-0x1.c64736p-1f}, 0x1.434bccp-1f}, //     1  
  {{-0x1.c62aaap-1f,-0x1.c62aa8p-1f}, 0x1.435bcap-1f}, //     1  
  {{-0x1.c60e10p-1f,-0x1.c60e0ep-1f}, 0x1.436bcap-1f}, //     1  
  {{-0x1.c5f174p-1f,-0x1.c5f172p-1f}, 0x1.437bc8p-1f}, //     1  
  {{-0x1.c5d4ccp-1f,-0x1.c5d4cap-1f}, 0x1.438bc8p-1f}, //     1  
  {{-0x1.c5b820p-1f,-0x1.c5b81ep-1f}, 0x1.439bc6p-1f}, //     1  
  {{-0x1.c59b68p-1f,-0x1.c59b66p-1f}, 0x1.43abc6p-1f}, //     1  
  {{-0x1.c57eacp-1f,-0x1.c57eacp-1f}, 0x1.43bbc4p-1f}, //     0  
  {{-0x1.c561e6p-1f,-0x1.c561e4p-1f}, 0x1.43cbc4p-1f}, //     1  
  {{-0x1.c5451cp-1f,-0x1.c5451ap-1f}, 0x1.43dbc2p-1f}, //     1  
  {{-0x1.c52846p-1f,-0x1.c52844p-1f}, 0x1.43ebc2p-1f}, //     1  
  {{-0x1.c50b6cp-1f,-0x1.c50b6cp-1f}, 0x1.43fbc0p-1f}, //     0  
  {{-0x1.c4ee88p-1f,-0x1.c4ee86p-1f}, 0x1.440bc0p-1f}, //     1  
  {{-0x1.c4d1a0p-1f,-0x1.c4d19ep-1f}, 0x1.441bbep-1f}, //     1  
  {{-0x1.c4b4acp-1f,-0x1.c4b4aap-1f}, 0x1.442bbep-1f}, //     1  
  {{-0x1.c497b4p-1f,-0x1.c497b2p-1f}, 0x1.443bbcp-1f}, //     1  
  {{-0x1.c47ab0p-1f,-0x1.c47aaep-1f}, 0x1.444bbcp-1f}, //     1  
  {{-0x1.c45daap-1f,-0x1.c45da8p-1f}, 0x1.445bbap-1f}, //     1  
  {{-0x1.c44096p-1f,-0x1.c44096p-1f}, 0x1.446bbap-1f}, //     0  
  {{-0x1.c42380p-1f,-0x1.c4237ep-1f}, 0x1.447bb8p-1f}, //     1  
  {{-0x1.c4065ep-1f,-0x1.c4065ep-1f}, 0x1.448bb8p-1f}, //     0  
  {{-0x1.c3e93ap-1f,-0x1.c3e938p-1f}, 0x1.449bb6p-1f}, //     1  
  {{-0x1.c3cc08p-1f,-0x1.c3cc06p-1f}, 0x1.44abb6p-1f}, //     1  
  {{-0x1.c3aed4p-1f,-0x1.c3aed2p-1f}, 0x1.44bbb4p-1f}, //     1  
  {{-0x1.c39194p-1f,-0x1.c39192p-1f}, 0x1.44cbb4p-1f}, //     1  
  {{-0x1.c37450p-1f,-0x1.c3744ep-1f}, 0x1.44dbb2p-1f}, //     1  
  {{-0x1.c35700p-1f,-0x1.c35700p-1f}, 0x1.44ebb2p-1f}, //     0  
  {{-0x1.c339aep-1f,-0x1.c339acp-1f}, 0x1.44fbb0p-1f}, //     1  
  {{-0x1.c31c50p-1f,-0x1.c31c4ep-1f}, 0x1.450bb0p-1f}, //     1  
  {{-0x1.c2feecp-1f,-0x1.c2feecp-1f}, 0x1.451baep-1f}, //     0  
  {{-0x1.c2e180p-1f,-0x1.c2e17ep-1f}, 0x1.452baep-1f}, //     1  
  {{-0x1.c2c40ep-1f,-0x1.c2c40cp-1f}, 0x1.453bacp-1f}, //     1  
  {{-0x1.c2a690p-1f,-0x1.c2a68ep-1f}, 0x1.454bacp-1f}, //     1  
  {{-0x1.c28910p-1f,-0x1.c2890ep-1f}, 0x1.455baap-1f}, //     1  
  {{-0x1.c26b84p-1f,-0x1.c26b82p-1f}, 0x1.456baap-1f}, //     1  
  {{-0x1.c24df4p-1f,-0x1.c24df2p-1f}, 0x1.457ba8p-1f}, //     1  
  {{-0x1.c23058p-1f,-0x1.c23056p-1f}, 0x1.458ba8p-1f}, //     1  
  {{-0x1.c212b8p-1f,-0x1.c212b6p-1f}, 0x1.459ba6p-1f}, //     1  
  {{-0x1.c1f50ep-1f,-0x1.c1f50cp-1f}, 0x1.45aba6p-1f}, //     1  
  {{-0x1.c1d760p-1f,-0x1.c1d75ep-1f}, 0x1.45bba4p-1f}, //     1  
  {{-0x1.c1b9a6p-1f,-0x1.c1b9a4p-1f}, 0x1.45cba4p-1f}, //     1  
  {{-0x1.c19be8p-1f,-0x1.c19be6p-1f}, 0x1.45dba2p-1f}, //     1  
  {{-0x1.c17e1ep-1f,-0x1.c17e1cp-1f}, 0x1.45eba2p-1f}, //     1  
  {{-0x1.c16052p-1f,-0x1.c16050p-1f}, 0x1.45fba0p-1f}, //     1  
  {{-0x1.c14278p-1f,-0x1.c14278p-1f}, 0x1.460ba0p-1f}, //     0  
  {{-0x1.c1249cp-1f,-0x1.c1249ap-1f}, 0x1.461b9ep-1f}, //     1  
  {{-0x1.c106b4p-1f,-0x1.c106b2p-1f}, 0x1.462b9ep-1f}, //     1  
  {{-0x1.c0e8cap-1f,-0x1.c0e8c8p-1f}, 0x1.463b9cp-1f}, //     1  
  {{-0x1.c0cad2p-1f,-0x1.c0cad0p-1f}, 0x1.464b9cp-1f}, //     1  
  {{-0x1.c0acd8p-1f,-0x1.c0acd6p-1f}, 0x1.465b9ap-1f}, //     1  
  {{-0x1.c08ed0p-1f,-0x1.c08ecep-1f}, 0x1.466b9ap-1f}, //     1  
  {{-0x1.c070c6p-1f,-0x1.c070c4p-1f}, 0x1.467b98p-1f}, //     1  
  {{-0x1.c052aep-1f,-0x1.c052acp-1f}, 0x1.468b9ap-1f}, //     1  
  {{-0x1.c03498p-1f,-0x1.c03496p-1f}, 0x1.469b96p-1f}, //     1  
  {{-0x1.c01672p-1f,-0x1.c01670p-1f}, 0x1.46ab96p-1f}, //     1  
  {{-0x1.bff84ap-1f,-0x1.bff848p-1f}, 0x1.46bb94p-1f}, //     1  
  {{-0x1.bfda16p-1f,-0x1.bfda14p-1f}, 0x1.46cb94p-1f}, //     1  
  {{-0x1.bfbbdep-1f,-0x1.bfbbdcp-1f}, 0x1.46db92p-1f}, //     1  
  {{-0x1.bf9d9ap-1f,-0x1.bf9d98p-1f}, 0x1.46eb92p-1f}, //     1  
  {{-0x1.bf7f52p-1f,-0x1.bf7f50p-1f}, 0x1.46fb90p-1f}, //     1  
  {{-0x1.bf6100p-1f,-0x1.bf60fep-1f}, 0x1.470b90p-1f}, //     1  
  {{-0x1.bf42a8p-1f,-0x1.bf42a6p-1f}, 0x1.471b8ep-1f}, //     1  
  {{-0x1.bf2446p-1f,-0x1.bf2444p-1f}, 0x1.472b8ep-1f}, //     1  
  {{-0x1.bf05e0p-1f,-0x1.bf05dep-1f}, 0x1.473b8cp-1f}, //     1  
  {{-0x1.bee76ep-1f,-0x1.bee76cp-1f}, 0x1.474b8cp-1f}, //     1  
  {{-0x1.bec8fap-1f,-0x1.bec8f8p-1f}, 0x1.475b8ap-1f}, //     1  
  {{-0x1.beaa78p-1f,-0x1.beaa76p-1f}, 0x1.476b8ap-1f}, //     1  
  {{-0x1.be8bf4p-1f,-0x1.be8bf2p-1f}, 0x1.477b88p-1f}, //     1  
  {{-0x1.be6d64p-1f,-0x1.be6d62p-1f}, 0x1.478b88p-1f}, //     1  
  {{-0x1.be4ed0p-1f,-0x1.be4ecep-1f}, 0x1.479b86p-1f}, //     1  
  {{-0x1.be3030p-1f,-0x1.be302ep-1f}, 0x1.47ab86p-1f}, //     1  
  {{-0x1.be118cp-1f,-0x1.be118ap-1f}, 0x1.47bb84p-1f}, //     1  
  {{-0x1.bdf2dep-1f,-0x1.bdf2dcp-1f}, 0x1.47cb84p-1f}, //     1  
  {{-0x1.bdd42ap-1f,-0x1.bdd428p-1f}, 0x1.47db82p-1f}, //     1  
  {{-0x1.bdb56cp-1f,-0x1.bdb56ap-1f}, 0x1.47eb82p-1f}, //     1  
  {{-0x1.bd96aap-1f,-0x1.bd96a8p-1f}, 0x1.47fb80p-1f}, //     1  
  {{-0x1.bd77dcp-1f,-0x1.bd77dap-1f}, 0x1.480b80p-1f}, //     1  
  {{-0x1.bd590ap-1f,-0x1.bd5908p-1f}, 0x1.481b7ep-1f}, //     1  
  {{-0x1.bd3a2ep-1f,-0x1.bd3a2cp-1f}, 0x1.482b7ep-1f}, //     1  
  {{-0x1.bd1b4cp-1f,-0x1.bd1b4cp-1f}, 0x1.483b7cp-1f}, //     0  
  {{-0x1.bcfc60p-1f,-0x1.bcfc5ep-1f}, 0x1.484b7cp-1f}, //     1  
  {{-0x1.bcdd70p-1f,-0x1.bcdd6ep-1f}, 0x1.485b7ap-1f}, //     1  
  {{-0x1.bcbe74p-1f,-0x1.bcbe72p-1f}, 0x1.486b7ap-1f}, //     1  
  {{-0x1.bc9f74p-1f,-0x1.bc9f72p-1f}, 0x1.487b78p-1f}, //     1  
  {{-0x1.bc806ap-1f,-0x1.bc8068p-1f}, 0x1.488b78p-1f}, //     1  
  {{-0x1.bc615ap-1f,-0x1.bc6158p-1f}, 0x1.489b76p-1f}, //     1  
  {{-0x1.bc4240p-1f,-0x1.bc423ep-1f}, 0x1.48ab76p-1f}, //     1  
  {{-0x1.bc2322p-1f,-0x1.bc2320p-1f}, 0x1.48bb74p-1f}, //     1  
  {{-0x1.bc03f8p-1f,-0x1.bc03f6p-1f}, 0x1.48cb74p-1f}, //     1  
  {{-0x1.bbe4cap-1f,-0x1.bbe4c8p-1f}, 0x1.48db72p-1f}, //     1  
  {{-0x1.bbc590p-1f,-0x1.bbc58ep-1f}, 0x1.48eb72p-1f}, //     1  
  {{-0x1.bba652p-1f,-0x1.bba652p-1f}, 0x1.48fb70p-1f}, //     0  
  {{-0x1.bb870ap-1f,-0x1.bb8708p-1f}, 0x1.490b70p-1f}, //     1  
  {{-0x1.bb67bep-1f,-0x1.bb67bcp-1f}, 0x1.491b6ep-1f}, //     1  
  {{-0x1.bb4866p-1f,-0x1.bb4864p-1f}, 0x1.492b6ep-1f}, //     1  
  {{-0x1.bb290ap-1f,-0x1.bb2908p-1f}, 0x1.493b6cp-1f}, //     1  
  {{-0x1.bb09a2p-1f,-0x1.bb09a0p-1f}, 0x1.494b6cp-1f}, //     1  
  {{-0x1.baea36p-1f,-0x1.baea34p-1f}, 0x1.495b6ap-1f}, //     1  
  {{-0x1.bacabep-1f,-0x1.bacabcp-1f}, 0x1.496b6ap-1f}, //     1  
  {{-0x1.baab44p-1f,-0x1.baab42p-1f}, 0x1.497b68p-1f}, //     1  
  {{-0x1.ba8bbep-1f,-0x1.ba8bbcp-1f}, 0x1.498b68p-1f}, //     1  
  {{-0x1.ba6c34p-1f,-0x1.ba6c32p-1f}, 0x1.499b66p-1f}, //     1  
  {{-0x1.ba4c9ep-1f,-0x1.ba4c9cp-1f}, 0x1.49ab66p-1f}, //     1  
  {{-0x1.ba2d04p-1f,-0x1.ba2d02p-1f}, 0x1.49bb64p-1f}, //     1  
  {{-0x1.ba0d5ep-1f,-0x1.ba0d5cp-1f}, 0x1.49cb64p-1f}, //     1  
  {{-0x1.b9edb4p-1f,-0x1.b9edb2p-1f}, 0x1.49db62p-1f}, //     1  
  {{-0x1.b9ce00p-1f,-0x1.b9cdfep-1f}, 0x1.49eb62p-1f}, //     1  
  {{-0x1.b9ae46p-1f,-0x1.b9ae44p-1f}, 0x1.49fb60p-1f}, //     1  
  {{-0x1.b98e82p-1f,-0x1.b98e80p-1f}, 0x1.4a0b60p-1f}, //     1  
  {{-0x1.b96ebap-1f,-0x1.b96eb8p-1f}, 0x1.4a1b5ep-1f}, //     1  
  {{-0x1.b94ee6p-1f,-0x1.b94ee4p-1f}, 0x1.4a2b5ep-1f}, //     1  
  {{-0x1.b92f12p-1f,-0x1.b92f10p-1f}, 0x1.4a3b5ap-1f}, //     1  
  {{-0x1.b90f2cp-1f,-0x1.b90f2ap-1f}, 0x1.4a4b5cp-1f}, //     1  
  {{-0x1.b8ef44p-1f,-0x1.b8ef42p-1f}, 0x1.4a5b5ap-1f}, //     1  
  {{-0x1.b8cf52p-1f,-0x1.b8cf50p-1f}, 0x1.4a6b5ap-1f}, //     1  
  {{-0x1.b8af5cp-1f,-0x1.b8af5ap-1f}, 0x1.4a7b58p-1f}, //     1  
  {{-0x1.b88f58p-1f,-0x1.b88f56p-1f}, 0x1.4a8b58p-1f}, //     1  
  {{-0x1.b86f52p-1f,-0x1.b86f50p-1f}, 0x1.4a9b56p-1f}, //     1  
  {{-0x1.b84f42p-1f,-0x1.b84f40p-1f}, 0x1.4aab56p-1f}, //     1  
  {{-0x1.b82f2cp-1f,-0x1.b82f2ap-1f}, 0x1.4abb54p-1f}, //     1  
  {{-0x1.b80f0ap-1f,-0x1.b80f08p-1f}, 0x1.4acb54p-1f}, //     1  
  {{-0x1.b7eee6p-1f,-0x1.b7eee4p-1f}, 0x1.4adb52p-1f}, //     1  
  {{-0x1.b7ceb4p-1f,-0x1.b7ceb2p-1f}, 0x1.4aeb52p-1f}, //     1  
  {{-0x1.b7ae80p-1f,-0x1.b7ae7ep-1f}, 0x1.4afb50p-1f}, //     1  
  {{-0x1.b78e40p-1f,-0x1.b78e3ep-1f}, 0x1.4b0b50p-1f}, //     1  
  {{-0x1.b76dfcp-1f,-0x1.b76dfap-1f}, 0x1.4b1b4ep-1f}, //     1  
  {{-0x1.b74dacp-1f,-0x1.b74daap-1f}, 0x1.4b2b4ep-1f}, //     1  
  {{-0x1.b72d58p-1f,-0x1.b72d56p-1f}, 0x1.4b3b4cp-1f}, //     1  
  {{-0x1.b70cfap-1f,-0x1.b70cf8p-1f}, 0x1.4b4b4cp-1f}, //     1  
  {{-0x1.b6ec96p-1f,-0x1.b6ec94p-1f}, 0x1.4b5b4ap-1f}, //     1  
  {{-0x1.b6cc28p-1f,-0x1.b6cc26p-1f}, 0x1.4b6b4ap-1f}, //     1  
  {{-0x1.b6abb4p-1f,-0x1.b6abb2p-1f}, 0x1.4b7b48p-1f}, //     1  
  {{-0x1.b68b36p-1f,-0x1.b68b34p-1f}, 0x1.4b8b48p-1f}, //     1  
  {{-0x1.b66ab4p-1f,-0x1.b66ab2p-1f}, 0x1.4b9b46p-1f}, //     1  
  {{-0x1.b64a26p-1f,-0x1.b64a24p-1f}, 0x1.4bab46p-1f}, //     1  
  {{-0x1.b62994p-1f,-0x1.b62992p-1f}, 0x1.4bbb44p-1f}, //     1  
  {{-0x1.b608f8p-1f,-0x1.b608f6p-1f}, 0x1.4bcb44p-1f}, //     1  
  {{-0x1.b5e856p-1f,-0x1.b5e854p-1f}, 0x1.4bdb42p-1f}, //     1  
  {{-0x1.b5c7aep-1f,-0x1.b5c7aap-1f}, 0x1.4beb40p-1f}, //     2  
  {{-0x1.b5a6f8p-1f,-0x1.b5a6f6p-1f}, 0x1.4bfb40p-1f}, //     1  
  {{-0x1.b5863cp-1f,-0x1.b5863ap-1f}, 0x1.4c0b40p-1f}, //     1  
  {{-0x1.b5657cp-1f,-0x1.b5657ap-1f}, 0x1.4c1b3ep-1f}, //     1  
  {{-0x1.b544b0p-1f,-0x1.b544aep-1f}, 0x1.4c2b3ep-1f}, //     1  
  {{-0x1.b523e0p-1f,-0x1.b523dep-1f}, 0x1.4c3b3cp-1f}, //     1  
  {{-0x1.b50304p-1f,-0x1.b50302p-1f}, 0x1.4c4b3cp-1f}, //     1  
  {{-0x1.b4e224p-1f,-0x1.b4e222p-1f}, 0x1.4c5b3ap-1f}, //     1  
  {{-0x1.b4c13ap-1f,-0x1.b4c136p-1f}, 0x1.4c6b3ap-1f}, //     2  
  {{-0x1.b4a04ap-1f,-0x1.b4a048p-1f}, 0x1.4c7b38p-1f}, //     1  
  {{-0x1.b47f50p-1f,-0x1.b47f4ep-1f}, 0x1.4c8b38p-1f}, //     1  
  {{-0x1.b45e50p-1f,-0x1.b45e4ep-1f}, 0x1.4c9b36p-1f}, //     1  
  {{-0x1.b43d46p-1f,-0x1.b43d44p-1f}, 0x1.4cab36p-1f}, //     1  
  {{-0x1.b41c38p-1f,-0x1.b41c36p-1f}, 0x1.4cbb34p-1f}, //     1  
  {{-0x1.b3fb1ep-1f,-0x1.b3fb1cp-1f}, 0x1.4ccb34p-1f}, //     1  
  {{-0x1.b3da00p-1f,-0x1.b3d9fep-1f}, 0x1.4cdb32p-1f}, //     1  
  {{-0x1.b3b8d6p-1f,-0x1.b3b8d4p-1f}, 0x1.4ceb32p-1f}, //     1  
  {{-0x1.b397aap-1f,-0x1.b397a8p-1f}, 0x1.4cfb30p-1f}, //     1  
  {{-0x1.b37670p-1f,-0x1.b3766ep-1f}, 0x1.4d0b30p-1f}, //     1  
  {{-0x1.b35534p-1f,-0x1.b35532p-1f}, 0x1.4d1b2ep-1f}, //     1  
  {{-0x1.b333eap-1f,-0x1.b333e8p-1f}, 0x1.4d2b2ep-1f}, //     1  
  {{-0x1.b3129ep-1f,-0x1.b3129cp-1f}, 0x1.4d3b2cp-1f}, //     1  
  {{-0x1.b2f146p-1f,-0x1.b2f144p-1f}, 0x1.4d4b2cp-1f}, //     1  
  {{-0x1.b2cfeap-1f,-0x1.b2cfe8p-1f}, 0x1.4d5b2ap-1f}, //     1  
  {{-0x1.b2ae82p-1f,-0x1.b2ae80p-1f}, 0x1.4d6b2ap-1f}, //     1  
  {{-0x1.b28d16p-1f,-0x1.b28d14p-1f}, 0x1.4d7b28p-1f}, //     1  
  {{-0x1.b26b9ep-1f,-0x1.b26b9cp-1f}, 0x1.4d8b28p-1f}, //     1  
  {{-0x1.b24a22p-1f,-0x1.b24a20p-1f}, 0x1.4d9b26p-1f}, //     1  
  {{-0x1.b2289cp-1f,-0x1.b2289ap-1f}, 0x1.4dab26p-1f}, //     1  
  {{-0x1.b20710p-1f,-0x1.b2070ep-1f}, 0x1.4dbb24p-1f}, //     1  
  {{-0x1.b1e57ap-1f,-0x1.b1e578p-1f}, 0x1.4dcb24p-1f}, //     1  
  {{-0x1.b1c3dep-1f,-0x1.b1c3dcp-1f}, 0x1.4ddb22p-1f}, //     1  
  {{-0x1.b1a238p-1f,-0x1.b1a236p-1f}, 0x1.4deb22p-1f}, //     1  
  {{-0x1.b1808ep-1f,-0x1.b1808cp-1f}, 0x1.4dfb20p-1f}, //     1  
  {{-0x1.b15ed8p-1f,-0x1.b15ed6p-1f}, 0x1.4e0b20p-1f}, //     1  
  {{-0x1.b13d1ep-1f,-0x1.b13d1cp-1f}, 0x1.4e1b1ep-1f}, //     1  
  {{-0x1.b11b58p-1f,-0x1.b11b56p-1f}, 0x1.4e2b1ep-1f}, //     1  
  {{-0x1.b0f98ep-1f,-0x1.b0f98cp-1f}, 0x1.4e3b1cp-1f}, //     1  
  {{-0x1.b0d7b8p-1f,-0x1.b0d7b6p-1f}, 0x1.4e4b1cp-1f}, //     1  
  {{-0x1.b0b5e0p-1f,-0x1.b0b5dep-1f}, 0x1.4e5b1ap-1f}, //     1  
  {{-0x1.b093fap-1f,-0x1.b093f8p-1f}, 0x1.4e6b1ap-1f}, //     1  
  {{-0x1.b07212p-1f,-0x1.b07210p-1f}, 0x1.4e7b18p-1f}, //     1  
  {{-0x1.b0501cp-1f,-0x1.b0501ap-1f}, 0x1.4e8b18p-1f}, //     1  
  {{-0x1.b02e24p-1f,-0x1.b02e22p-1f}, 0x1.4e9b16p-1f}, //     1  
  {{-0x1.b00c20p-1f,-0x1.b00c1cp-1f}, 0x1.4eab16p-1f}, //     2  
  {{-0x1.afea1cp-1f,-0x1.afea18p-1f}, 0x1.4ebb12p-1f}, //     2  
  {{-0x1.afc802p-1f,-0x1.afc800p-1f}, 0x1.4ecb14p-1f}, //     1  
  {{-0x1.afa5eap-1f,-0x1.afa5e8p-1f}, 0x1.4edb12p-1f}, //     1  
  {{-0x1.af83c6p-1f,-0x1.af83c4p-1f}, 0x1.4eeb12p-1f}, //     1  
  {{-0x1.af61a0p-1f,-0x1.af619cp-1f}, 0x1.4efb10p-1f}, //     2  
  {{-0x1.af3f6cp-1f,-0x1.af3f6ap-1f}, 0x1.4f0b10p-1f}, //     1  
  {{-0x1.af1d34p-1f,-0x1.af1d32p-1f}, 0x1.4f1b0ep-1f}, //     1  
  {{-0x1.aefaf0p-1f,-0x1.aefaeep-1f}, 0x1.4f2b0ep-1f}, //     1  
  {{-0x1.aed8aap-1f,-0x1.aed8a8p-1f}, 0x1.4f3b0cp-1f}, //     1  
  {{-0x1.aeb656p-1f,-0x1.aeb654p-1f}, 0x1.4f4b0cp-1f}, //     1  
  {{-0x1.ae9400p-1f,-0x1.ae93fep-1f}, 0x1.4f5b0ap-1f}, //     1  
  {{-0x1.ae719ep-1f,-0x1.ae719cp-1f}, 0x1.4f6b0ap-1f}, //     1  
  {{-0x1.ae4f38p-1f,-0x1.ae4f34p-1f}, 0x1.4f7b08p-1f}, //     2  
  {{-0x1.ae2cc4p-1f,-0x1.ae2cc2p-1f}, 0x1.4f8b08p-1f}, //     1  
  {{-0x1.ae0a4ep-1f,-0x1.ae0a4cp-1f}, 0x1.4f9b06p-1f}, //     1  
  {{-0x1.ade7ccp-1f,-0x1.ade7cap-1f}, 0x1.4fab06p-1f}, //     1  
  {{-0x1.adc546p-1f,-0x1.adc544p-1f}, 0x1.4fbb04p-1f}, //     1  
  {{-0x1.ada2b4p-1f,-0x1.ada2b2p-1f}, 0x1.4fcb04p-1f}, //     1  
  {{-0x1.ad801ep-1f,-0x1.ad801cp-1f}, 0x1.4fdb02p-1f}, //     1  
  {{-0x1.ad5d7ep-1f,-0x1.ad5d7ap-1f}, 0x1.4feb02p-1f}, //     2  
  {{-0x1.ad3ad8p-1f,-0x1.ad3ad6p-1f}, 0x1.4ffb00p-1f}, //     1  
  {{-0x1.ad1826p-1f,-0x1.ad1824p-1f}, 0x1.500b00p-1f}, //     1  
  {{-0x1.acf572p-1f,-0x1.acf56ep-1f}, 0x1.501afep-1f}, //     2  
  {{-0x1.acd2b0p-1f,-0x1.acd2aep-1f}, 0x1.502afep-1f}, //     1  
  {{-0x1.acafecp-1f,-0x1.acafe8p-1f}, 0x1.503afcp-1f}, //     2  
  {{-0x1.ac8d1ap-1f,-0x1.ac8d18p-1f}, 0x1.504afcp-1f}, //     1  
  {{-0x1.ac6a46p-1f,-0x1.ac6a44p-1f}, 0x1.505afap-1f}, //     1  
  {{-0x1.ac4766p-1f,-0x1.ac4762p-1f}, 0x1.506afap-1f}, //     2  
  {{-0x1.ac2480p-1f,-0x1.ac247ep-1f}, 0x1.507af8p-1f}, //     1  
  {{-0x1.ac0190p-1f,-0x1.ac018ep-1f}, 0x1.508af8p-1f}, //     1  
  {{-0x1.abde9cp-1f,-0x1.abde9ap-1f}, 0x1.509af6p-1f}, //     1  
  {{-0x1.abbb9cp-1f,-0x1.abbb9ap-1f}, 0x1.50aaf6p-1f}, //     1  
  {{-0x1.ab9898p-1f,-0x1.ab9896p-1f}, 0x1.50baf4p-1f}, //     1  
  {{-0x1.ab7588p-1f,-0x1.ab7586p-1f}, 0x1.50caf4p-1f}, //     1  
  {{-0x1.ab5274p-1f,-0x1.ab5272p-1f}, 0x1.50daf2p-1f}, //     1  
  {{-0x1.ab2f54p-1f,-0x1.ab2f52p-1f}, 0x1.50eaf2p-1f}, //     1  
  {{-0x1.ab0c32p-1f,-0x1.ab0c30p-1f}, 0x1.50faf0p-1f}, //     1  
  {{-0x1.aae902p-1f,-0x1.aae900p-1f}, 0x1.510af0p-1f}, //     1  
  {{-0x1.aac5cep-1f,-0x1.aac5ccp-1f}, 0x1.511aeep-1f}, //     1  
  {{-0x1.aaa290p-1f,-0x1.aaa28cp-1f}, 0x1.512aeep-1f}, //     2  
  {{-0x1.aa7f4cp-1f,-0x1.aa7f4ap-1f}, 0x1.513aecp-1f}, //     1  
  {{-0x1.aa5bfep-1f,-0x1.aa5bfap-1f}, 0x1.514aecp-1f}, //     2  
  {{-0x1.aa38aap-1f,-0x1.aa38a8p-1f}, 0x1.515aeap-1f}, //     1  
  {{-0x1.aa154cp-1f,-0x1.aa1548p-1f}, 0x1.516aeap-1f}, //     2  
  {{-0x1.a9f1e8p-1f,-0x1.a9f1e6p-1f}, 0x1.517ae8p-1f}, //     1  
  {{-0x1.a9ce7ap-1f,-0x1.a9ce78p-1f}, 0x1.518ae8p-1f}, //     1  
  {{-0x1.a9ab08p-1f,-0x1.a9ab06p-1f}, 0x1.519ae6p-1f}, //     1  
  {{-0x1.a98788p-1f,-0x1.a98786p-1f}, 0x1.51aae6p-1f}, //     1  
  {{-0x1.a96406p-1f,-0x1.a96404p-1f}, 0x1.51bae4p-1f}, //     1  
  {{-0x1.a94078p-1f,-0x1.a94076p-1f}, 0x1.51cae4p-1f}, //     1  
  {{-0x1.a91ce6p-1f,-0x1.a91ce4p-1f}, 0x1.51dae2p-1f}, //     1  
  {{-0x1.a8f948p-1f,-0x1.a8f946p-1f}, 0x1.51eae2p-1f}, //     1  
  {{-0x1.a8d5a6p-1f,-0x1.a8d5a4p-1f}, 0x1.51fae0p-1f}, //     1  
  {{-0x1.a8b1f8p-1f,-0x1.a8b1f6p-1f}, 0x1.520ae0p-1f}, //     1  
  {{-0x1.a88e46p-1f,-0x1.a88e44p-1f}, 0x1.521adep-1f}, //     1  
  {{-0x1.a86a88p-1f,-0x1.a86a86p-1f}, 0x1.522adep-1f}, //     1  
  {{-0x1.a846c6p-1f,-0x1.a846c4p-1f}, 0x1.523adcp-1f}, //     1  
  {{-0x1.a822fap-1f,-0x1.a822f6p-1f}, 0x1.524adcp-1f}, //     2  
  {{-0x1.a7ff28p-1f,-0x1.a7ff26p-1f}, 0x1.525adap-1f}, //     1  
  {{-0x1.a7db4ap-1f,-0x1.a7db48p-1f}, 0x1.526adap-1f}, //     1  
  {{-0x1.a7b768p-1f,-0x1.a7b766p-1f}, 0x1.527ad8p-1f}, //     1  
  {{-0x1.a7937cp-1f,-0x1.a79378p-1f}, 0x1.528ad8p-1f}, //     2  
  {{-0x1.a76f8ap-1f,-0x1.a76f88p-1f}, 0x1.529ad6p-1f}, //     1  
  {{-0x1.a74b8cp-1f,-0x1.a74b8ap-1f}, 0x1.52aad6p-1f}, //     1  
  {{-0x1.a7278cp-1f,-0x1.a7278ap-1f}, 0x1.52bad4p-1f}, //     1  
  {{-0x1.a7037ep-1f,-0x1.a7037cp-1f}, 0x1.52cad4p-1f}, //     1  
  {{-0x1.a6df6ep-1f,-0x1.a6df6cp-1f}, 0x1.52dad2p-1f}, //     1  
  {{-0x1.a6bb50p-1f,-0x1.a6bb4ep-1f}, 0x1.52ead2p-1f}, //     1  
  {{-0x1.a69730p-1f,-0x1.a6972ep-1f}, 0x1.52fad0p-1f}, //     1  
  {{-0x1.a67302p-1f,-0x1.a67300p-1f}, 0x1.530ad0p-1f}, //     1  
  {{-0x1.a64ed2p-1f,-0x1.a64ed0p-1f}, 0x1.531acep-1f}, //     1  
  {{-0x1.a62a96p-1f,-0x1.a62a92p-1f}, 0x1.532acep-1f}, //     2  
  {{-0x1.a60656p-1f,-0x1.a60652p-1f}, 0x1.533accp-1f}, //     2  
  {{-0x1.a5e208p-1f,-0x1.a5e206p-1f}, 0x1.534accp-1f}, //     1  
  {{-0x1.a5bdb8p-1f,-0x1.a5bdb6p-1f}, 0x1.535acap-1f}, //     1  
  {{-0x1.a5995cp-1f,-0x1.a59958p-1f}, 0x1.536acap-1f}, //     2  
  {{-0x1.a574fap-1f,-0x1.a574f8p-1f}, 0x1.537ac8p-1f}, //     1  
  {{-0x1.a5508ep-1f,-0x1.a5508cp-1f}, 0x1.538ac8p-1f}, //     1  
  {{-0x1.a52c1ep-1f,-0x1.a52c1cp-1f}, 0x1.539ac6p-1f}, //     1  
  {{-0x1.a507a2p-1f,-0x1.a507a0p-1f}, 0x1.53aac6p-1f}, //     1  
  {{-0x1.a4e322p-1f,-0x1.a4e320p-1f}, 0x1.53bac4p-1f}, //     1  
  {{-0x1.a4be96p-1f,-0x1.a4be92p-1f}, 0x1.53cac4p-1f}, //     2  
  {{-0x1.a49a06p-1f,-0x1.a49a02p-1f}, 0x1.53dac2p-1f}, //     2  
  {{-0x1.a47568p-1f,-0x1.a47566p-1f}, 0x1.53eac2p-1f}, //     1  
  {{-0x1.a450c8p-1f,-0x1.a450c6p-1f}, 0x1.53fac0p-1f}, //     1  
  {{-0x1.a42c1cp-1f,-0x1.a42c1ap-1f}, 0x1.540ac0p-1f}, //     1  
  {{-0x1.a4076cp-1f,-0x1.a4076ap-1f}, 0x1.541abep-1f}, //     1  
  {{-0x1.a3e2b0p-1f,-0x1.a3e2aep-1f}, 0x1.542abep-1f}, //     1  
  {{-0x1.a3bdf0p-1f,-0x1.a3bdeep-1f}, 0x1.543abcp-1f}, //     1  
  {{-0x1.a39924p-1f,-0x1.a39922p-1f}, 0x1.544abcp-1f}, //     1  
  {{-0x1.a37456p-1f,-0x1.a37452p-1f}, 0x1.545abap-1f}, //     2  
  {{-0x1.a34f78p-1f,-0x1.a34f76p-1f}, 0x1.546abap-1f}, //     1  
  {{-0x1.a32a9ap-1f,-0x1.a32a96p-1f}, 0x1.547ab8p-1f}, //     2  
  {{-0x1.a305aep-1f,-0x1.a305aap-1f}, 0x1.548ab8p-1f}, //     2  
  {{-0x1.a2e0bep-1f,-0x1.a2e0bap-1f}, 0x1.549ab6p-1f}, //     2  
  {{-0x1.a2bbc2p-1f,-0x1.a2bbbep-1f}, 0x1.54aab6p-1f}, //     2  
  {{-0x1.a296c2p-1f,-0x1.a296c0p-1f}, 0x1.54bab4p-1f}, //     1  
  {{-0x1.a271b6p-1f,-0x1.a271b4p-1f}, 0x1.54cab4p-1f}, //     1  
  {{-0x1.a24ca6p-1f,-0x1.a24ca4p-1f}, 0x1.54dab2p-1f}, //     1  
  {{-0x1.a2278ap-1f,-0x1.a22788p-1f}, 0x1.54eab2p-1f}, //     1  
  {{-0x1.a2026cp-1f,-0x1.a20268p-1f}, 0x1.54fab0p-1f}, //     2  
  {{-0x1.a1dd40p-1f,-0x1.a1dd3cp-1f}, 0x1.550ab0p-1f}, //     2  
  {{-0x1.a1b810p-1f,-0x1.a1b80ep-1f}, 0x1.551aaep-1f}, //     1  
  {{-0x1.a192d4p-1f,-0x1.a192d0p-1f}, 0x1.552aaep-1f}, //     2  
  {{-0x1.a16d94p-1f,-0x1.a16d92p-1f}, 0x1.553aacp-1f}, //     1  
  {{-0x1.a14848p-1f,-0x1.a14846p-1f}, 0x1.554aacp-1f}, //     1  
  {{-0x1.a122f8p-1f,-0x1.a122f6p-1f}, 0x1.555aaap-1f}, //     1  
  {{-0x1.a0fd9cp-1f,-0x1.a0fd9ap-1f}, 0x1.556aaap-1f}, //     1  
  {{-0x1.a0d83ep-1f,-0x1.a0d83ap-1f}, 0x1.557aa8p-1f}, //     2  
  {{-0x1.a0b2d2p-1f,-0x1.a0b2cep-1f}, 0x1.558aa8p-1f}, //     2  
  {{-0x1.a08d62p-1f,-0x1.a08d60p-1f}, 0x1.559aa6p-1f}, //     1  
  {{-0x1.a067e6p-1f,-0x1.a067e4p-1f}, 0x1.55aaa6p-1f}, //     1  
  {{-0x1.a04266p-1f,-0x1.a04264p-1f}, 0x1.55baa4p-1f}, //     1  
  {{-0x1.a01cdap-1f,-0x1.a01cd8p-1f}, 0x1.55caa4p-1f}, //     1  
  {{-0x1.9ff74cp-1f,-0x1.9ff748p-1f}, 0x1.55daa2p-1f}, //     2  
  {{-0x1.9fd1b0p-1f,-0x1.9fd1acp-1f}, 0x1.55eaa2p-1f}, //     2  
  {{-0x1.9fac10p-1f,-0x1.9fac0ep-1f}, 0x1.55faa0p-1f}, //     1  
  {{-0x1.9f8664p-1f,-0x1.9f8662p-1f}, 0x1.560aa0p-1f}, //     1  
  {{-0x1.9f60b4p-1f,-0x1.9f60b2p-1f}, 0x1.561a9ep-1f}, //     1  
  {{-0x1.9f3af8p-1f,-0x1.9f3af6p-1f}, 0x1.562a9ep-1f}, //     1  
  {{-0x1.9f153ap-1f,-0x1.9f1536p-1f}, 0x1.563a9cp-1f}, //     2  
  {{-0x1.9eef6ep-1f,-0x1.9eef6ap-1f}, 0x1.564a9cp-1f}, //     2  
  {{-0x1.9ec99ep-1f,-0x1.9ec99cp-1f}, 0x1.565a9ap-1f}, //     1  
  {{-0x1.9ea3c2p-1f,-0x1.9ea3bep-1f}, 0x1.566a9ap-1f}, //     2  
  {{-0x1.9e7de2p-1f,-0x1.9e7de0p-1f}, 0x1.567a98p-1f}, //     1  
  {{-0x1.9e57f6p-1f,-0x1.9e57f4p-1f}, 0x1.568a98p-1f}, //     1  
  {{-0x1.9e3206p-1f,-0x1.9e3204p-1f}, 0x1.569a96p-1f}, //     1  
  {{-0x1.9e0c0ap-1f,-0x1.9e0c08p-1f}, 0x1.56aa96p-1f}, //     1  
  {{-0x1.9de60ap-1f,-0x1.9de608p-1f}, 0x1.56ba94p-1f}, //     1  
  {{-0x1.9dbffep-1f,-0x1.9dbffcp-1f}, 0x1.56ca94p-1f}, //     1  
  {{-0x1.9d99eep-1f,-0x1.9d99ecp-1f}, 0x1.56da92p-1f}, //     1  
  {{-0x1.9d73d2p-1f,-0x1.9d73d0p-1f}, 0x1.56ea92p-1f}, //     1  
  {{-0x1.9d4db2p-1f,-0x1.9d4db0p-1f}, 0x1.56fa90p-1f}, //     1  
  {{-0x1.9d2786p-1f,-0x1.9d2784p-1f}, 0x1.570a90p-1f}, //     1  
  {{-0x1.9d0156p-1f,-0x1.9d0154p-1f}, 0x1.571a8ep-1f}, //     1  
  {{-0x1.9cdb1ap-1f,-0x1.9cdb18p-1f}, 0x1.572a8ep-1f}, //     1  
  {{-0x1.9cb4dap-1f,-0x1.9cb4d8p-1f}, 0x1.573a8cp-1f}, //     1  
  {{-0x1.9c8e8ep-1f,-0x1.9c8e8ap-1f}, 0x1.574a8cp-1f}, //     2  
  {{-0x1.9c683ep-1f,-0x1.9c683ap-1f}, 0x1.575a8ap-1f}, //     2  
  {{-0x1.9c41e0p-1f,-0x1.9c41dep-1f}, 0x1.576a8ap-1f}, //     1  
  {{-0x1.9c1b80p-1f,-0x1.9c1b7ep-1f}, 0x1.577a88p-1f}, //     1  
  {{-0x1.9bf514p-1f,-0x1.9bf512p-1f}, 0x1.578a88p-1f}, //     1  
  {{-0x1.9bcea4p-1f,-0x1.9bcea2p-1f}, 0x1.579a86p-1f}, //     1  
  {{-0x1.9ba828p-1f,-0x1.9ba824p-1f}, 0x1.57aa86p-1f}, //     2  
  {{-0x1.9b81a6p-1f,-0x1.9b81a4p-1f}, 0x1.57ba84p-1f}, //     1  
  {{-0x1.9b5b1ap-1f,-0x1.9b5b18p-1f}, 0x1.57ca84p-1f}, //     1  
  {{-0x1.9b348ap-1f,-0x1.9b3486p-1f}, 0x1.57da82p-1f}, //     2  
  {{-0x1.9b0decp-1f,-0x1.9b0deap-1f}, 0x1.57ea82p-1f}, //     1  
  {{-0x1.9ae74cp-1f,-0x1.9ae74ap-1f}, 0x1.57fa80p-1f}, //     1  
  {{-0x1.9ac09ep-1f,-0x1.9ac09cp-1f}, 0x1.580a80p-1f}, //     1  
  {{-0x1.9a99eep-1f,-0x1.9a99ecp-1f}, 0x1.581a7ep-1f}, //     1  
  {{-0x1.9a7330p-1f,-0x1.9a732ep-1f}, 0x1.582a7ep-1f}, //     1  
  {{-0x1.9a4c70p-1f,-0x1.9a4c6ep-1f}, 0x1.583a7cp-1f}, //     1  
  {{-0x1.9a25a2p-1f,-0x1.9a25a0p-1f}, 0x1.584a7cp-1f}, //     1  
  {{-0x1.99fed2p-1f,-0x1.99fed0p-1f}, 0x1.585a7ap-1f}, //     1  
  {{-0x1.99d7f4p-1f,-0x1.99d7f2p-1f}, 0x1.586a7ap-1f}, //     1  
  {{-0x1.99b114p-1f,-0x1.99b110p-1f}, 0x1.587a78p-1f}, //     2  
  {{-0x1.998a26p-1f,-0x1.998a22p-1f}, 0x1.588a78p-1f}, //     2  
  {{-0x1.996334p-1f,-0x1.996332p-1f}, 0x1.589a76p-1f}, //     1  
  {{-0x1.993c36p-1f,-0x1.993c34p-1f}, 0x1.58aa76p-1f}, //     1  
  {{-0x1.991536p-1f,-0x1.991532p-1f}, 0x1.58ba74p-1f}, //     2  
  {{-0x1.98ee28p-1f,-0x1.98ee24p-1f}, 0x1.58ca74p-1f}, //     2  
  {{-0x1.98c716p-1f,-0x1.98c714p-1f}, 0x1.58da72p-1f}, //     1  
  {{-0x1.989ff8p-1f,-0x1.989ff6p-1f}, 0x1.58ea72p-1f}, //     1  
  {{-0x1.9878d6p-1f,-0x1.9878d4p-1f}, 0x1.58fa70p-1f}, //     1  
  {{-0x1.9851a8p-1f,-0x1.9851a6p-1f}, 0x1.590a70p-1f}, //     1  
  {{-0x1.982a76p-1f,-0x1.982a74p-1f}, 0x1.591a6ep-1f}, //     1  
  {{-0x1.980338p-1f,-0x1.980336p-1f}, 0x1.592a6ep-1f}, //     1  
  {{-0x1.97dbf6p-1f,-0x1.97dbf4p-1f}, 0x1.593a6cp-1f}, //     1  
  {{-0x1.97b4a8p-1f,-0x1.97b4a4p-1f}, 0x1.594a6cp-1f}, //     2  
  {{-0x1.978d56p-1f,-0x1.978d52p-1f}, 0x1.595a6ap-1f}, //     2  
  {{-0x1.9765f6p-1f,-0x1.9765f4p-1f}, 0x1.596a6ap-1f}, //     1  
  {{-0x1.973e94p-1f,-0x1.973e92p-1f}, 0x1.597a68p-1f}, //     1  
  {{-0x1.971726p-1f,-0x1.971722p-1f}, 0x1.598a68p-1f}, //     2  
  {{-0x1.96efb4p-1f,-0x1.96efb0p-1f}, 0x1.599a66p-1f}, //     2  
  {{-0x1.96c834p-1f,-0x1.96c830p-1f}, 0x1.59aa66p-1f}, //     2  
  {{-0x1.96a0b2p-1f,-0x1.96a0aep-1f}, 0x1.59ba64p-1f}, //     2  
  {{-0x1.967922p-1f,-0x1.96791ep-1f}, 0x1.59ca64p-1f}, //     2  
  {{-0x1.965190p-1f,-0x1.96518cp-1f}, 0x1.59da62p-1f}, //     2  
  {{-0x1.9629f0p-1f,-0x1.9629ecp-1f}, 0x1.59ea62p-1f}, //     2  
  {{-0x1.96024cp-1f,-0x1.96024ap-1f}, 0x1.59fa60p-1f}, //     1  
  {{-0x1.95da9cp-1f,-0x1.95da9ap-1f}, 0x1.5a0a60p-1f}, //     1  
  {{-0x1.95b2eap-1f,-0x1.95b2e6p-1f}, 0x1.5a1a5ep-1f}, //     2  
  {{-0x1.958b2ap-1f,-0x1.958b26p-1f}, 0x1.5a2a5ep-1f}, //     2  
  {{-0x1.956366p-1f,-0x1.956362p-1f}, 0x1.5a3a5cp-1f}, //     2  
  {{-0x1.953b96p-1f,-0x1.953b92p-1f}, 0x1.5a4a5cp-1f}, //     2  
  {{-0x1.9513c2p-1f,-0x1.9513bep-1f}, 0x1.5a5a5ap-1f}, //     2  
  {{-0x1.94ebe2p-1f,-0x1.94ebdep-1f}, 0x1.5a6a5ap-1f}, //     2  
  {{-0x1.94c3fep-1f,-0x1.94c3fap-1f}, 0x1.5a7a58p-1f}, //     2  
  {{-0x1.949c0cp-1f,-0x1.949c0ap-1f}, 0x1.5a8a58p-1f}, //     1  
  {{-0x1.947418p-1f,-0x1.947416p-1f}, 0x1.5a9a56p-1f}, //     1  
  {{-0x1.944c18p-1f,-0x1.944c14p-1f}, 0x1.5aaa56p-1f}, //     2  
  {{-0x1.942414p-1f,-0x1.942410p-1f}, 0x1.5aba54p-1f}, //     2  
  {{-0x1.93fc02p-1f,-0x1.93fc00p-1f}, 0x1.5aca54p-1f}, //     1  
  {{-0x1.93d3eep-1f,-0x1.93d3eap-1f}, 0x1.5ada52p-1f}, //     2  
  {{-0x1.93abccp-1f,-0x1.93abc8p-1f}, 0x1.5aea52p-1f}, //     2  
  {{-0x1.9383a8p-1f,-0x1.9383a4p-1f}, 0x1.5afa50p-1f}, //     2  
  {{-0x1.935b76p-1f,-0x1.935b72p-1f}, 0x1.5b0a50p-1f}, //     2  
  {{-0x1.933340p-1f,-0x1.93333ep-1f}, 0x1.5b1a4ep-1f}, //     1  
  {{-0x1.930afep-1f,-0x1.930afcp-1f}, 0x1.5b2a4ep-1f}, //     1  
  {{-0x1.92e2bap-1f,-0x1.92e2b6p-1f}, 0x1.5b3a4cp-1f}, //     2  
  {{-0x1.92ba66p-1f,-0x1.92ba64p-1f}, 0x1.5b4a4cp-1f}, //     1  
  {{-0x1.929212p-1f,-0x1.92920ep-1f}, 0x1.5b5a4ap-1f}, //     2  
  {{-0x1.9269aep-1f,-0x1.9269acp-1f}, 0x1.5b6a4ap-1f}, //     1  
  {{-0x1.924148p-1f,-0x1.924146p-1f}, 0x1.5b7a48p-1f}, //     1  
  {{-0x1.9218d6p-1f,-0x1.9218d2p-1f}, 0x1.5b8a48p-1f}, //     2  
  {{-0x1.91f060p-1f,-0x1.91f05cp-1f}, 0x1.5b9a46p-1f}, //     2  
  {{-0x1.91c7dcp-1f,-0x1.91c7dap-1f}, 0x1.5baa46p-1f}, //     1  
  {{-0x1.919f56p-1f,-0x1.919f54p-1f}, 0x1.5bba44p-1f}, //     1  
  {{-0x1.9176c2p-1f,-0x1.9176c0p-1f}, 0x1.5bca44p-1f}, //     1  
  {{-0x1.914e2cp-1f,-0x1.914e2ap-1f}, 0x1.5bda42p-1f}, //     1  
  {{-0x1.912588p-1f,-0x1.912586p-1f}, 0x1.5bea42p-1f}, //     1  
  {{-0x1.90fce2p-1f,-0x1.90fcdep-1f}, 0x1.5bfa40p-1f}, //     2  
  {{-0x1.90d42ep-1f,-0x1.90d42ap-1f}, 0x1.5c0a40p-1f}, //     2  
  {{-0x1.90ab76p-1f,-0x1.90ab72p-1f}, 0x1.5c1a3ep-1f}, //     2  
  {{-0x1.9082b2p-1f,-0x1.9082aep-1f}, 0x1.5c2a3ep-1f}, //     2  
  {{-0x1.9059eap-1f,-0x1.9059e6p-1f}, 0x1.5c3a3cp-1f}, //     2  
  {{-0x1.903116p-1f,-0x1.903112p-1f}, 0x1.5c4a3cp-1f}, //     2  
  {{-0x1.90083ep-1f,-0x1.90083ap-1f}, 0x1.5c5a3ap-1f}, //     2  
  {{-0x1.8fdf58p-1f,-0x1.8fdf56p-1f}, 0x1.5c6a3ap-1f}, //     1  
  {{-0x1.8fb670p-1f,-0x1.8fb66ep-1f}, 0x1.5c7a38p-1f}, //     1  
  {{-0x1.8f8d7ap-1f,-0x1.8f8d78p-1f}, 0x1.5c8a38p-1f}, //     1  
  {{-0x1.8f6482p-1f,-0x1.8f6480p-1f}, 0x1.5c9a36p-1f}, //     1  
  {{-0x1.8f3b7cp-1f,-0x1.8f3b7ap-1f}, 0x1.5caa36p-1f}, //     1  
  {{-0x1.8f1274p-1f,-0x1.8f1270p-1f}, 0x1.5cba34p-1f}, //     2  
  {{-0x1.8ee95ep-1f,-0x1.8ee95ap-1f}, 0x1.5cca34p-1f}, //     2  
  {{-0x1.8ec044p-1f,-0x1.8ec042p-1f}, 0x1.5cda32p-1f}, //     1  
  {{-0x1.8e971ep-1f,-0x1.8e971ap-1f}, 0x1.5cea32p-1f}, //     2  
  {{-0x1.8e6df4p-1f,-0x1.8e6df2p-1f}, 0x1.5cfa30p-1f}, //     1  
  {{-0x1.8e44bep-1f,-0x1.8e44bap-1f}, 0x1.5d0a30p-1f}, //     2  
  {{-0x1.8e1b84p-1f,-0x1.8e1b80p-1f}, 0x1.5d1a2ep-1f}, //     2  
  {{-0x1.8df23cp-1f,-0x1.8df23ap-1f}, 0x1.5d2a2ep-1f}, //     1  
  {{-0x1.8dc8f2p-1f,-0x1.8dc8f0p-1f}, 0x1.5d3a2cp-1f}, //     1  
  {{-0x1.8d9f9cp-1f,-0x1.8d9f98p-1f}, 0x1.5d4a2cp-1f}, //     2  
  {{-0x1.8d7640p-1f,-0x1.8d763ep-1f}, 0x1.5d5a2ap-1f}, //     1  
  {{-0x1.8d4cd8p-1f,-0x1.8d4cd6p-1f}, 0x1.5d6a2ap-1f}, //     1  
  {{-0x1.8d236ep-1f,-0x1.8d236ap-1f}, 0x1.5d7a28p-1f}, //     2  
  {{-0x1.8cf9f6p-1f,-0x1.8cf9f2p-1f}, 0x1.5d8a28p-1f}, //     2  
  {{-0x1.8cd07ap-1f,-0x1.8cd076p-1f}, 0x1.5d9a26p-1f}, //     2  
  {{-0x1.8ca6f2p-1f,-0x1.8ca6eep-1f}, 0x1.5daa26p-1f}, //     2  
  {{-0x1.8c7d66p-1f,-0x1.8c7d62p-1f}, 0x1.5dba24p-1f}, //     2  
  {{-0x1.8c53ccp-1f,-0x1.8c53cap-1f}, 0x1.5dca24p-1f}, //     1  
  {{-0x1.8c2a30p-1f,-0x1.8c2a2ep-1f}, 0x1.5dda22p-1f}, //     1  
  {{-0x1.8c0088p-1f,-0x1.8c0084p-1f}, 0x1.5dea22p-1f}, //     2  
  {{-0x1.8bd6dap-1f,-0x1.8bd6d8p-1f}, 0x1.5dfa20p-1f}, //     1  
  {{-0x1.8bad22p-1f,-0x1.8bad1ep-1f}, 0x1.5e0a20p-1f}, //     2  
  {{-0x1.8b8364p-1f,-0x1.8b8362p-1f}, 0x1.5e1a1ep-1f}, //     1  
  {{-0x1.8b599ap-1f,-0x1.8b5996p-1f}, 0x1.5e2a1ep-1f}, //     2  
  {{-0x1.8b2fccp-1f,-0x1.8b2fcap-1f}, 0x1.5e3a1cp-1f}, //     1  
  {{-0x1.8b05f2p-1f,-0x1.8b05eep-1f}, 0x1.5e4a1cp-1f}, //     2  
  {{-0x1.8adc14p-1f,-0x1.8adc12p-1f}, 0x1.5e5a1ap-1f}, //     1  
  {{-0x1.8ab22ap-1f,-0x1.8ab226p-1f}, 0x1.5e6a1ap-1f}, //     2  
  {{-0x1.8a883cp-1f,-0x1.8a8838p-1f}, 0x1.5e7a18p-1f}, //     2  
  {{-0x1.8a5e40p-1f,-0x1.8a5e3cp-1f}, 0x1.5e8a18p-1f}, //     2  
  {{-0x1.8a3442p-1f,-0x1.8a343ep-1f}, 0x1.5e9a16p-1f}, //     2  
  {{-0x1.8a0a36p-1f,-0x1.8a0a32p-1f}, 0x1.5eaa16p-1f}, //     2  
  {{-0x1.89e026p-1f,-0x1.89e024p-1f}, 0x1.5eba14p-1f}, //     1  
  {{-0x1.89b60ap-1f,-0x1.89b606p-1f}, 0x1.5eca14p-1f}, //     2  
  {{-0x1.898beap-1f,-0x1.898be8p-1f}, 0x1.5eda12p-1f}, //     1  
  {{-0x1.8961bep-1f,-0x1.8961bap-1f}, 0x1.5eea12p-1f}, //     2  
  {{-0x1.89378ep-1f,-0x1.89378cp-1f}, 0x1.5efa10p-1f}, //     1  
  {{-0x1.890d52p-1f,-0x1.890d4ep-1f}, 0x1.5f0a10p-1f}, //     2  
  {{-0x1.88e310p-1f,-0x1.88e30ep-1f}, 0x1.5f1a0ep-1f}, //     1  
  {{-0x1.88b8c4p-1f,-0x1.88b8c0p-1f}, 0x1.5f2a0ep-1f}, //     2  
  {{-0x1.888e72p-1f,-0x1.888e70p-1f}, 0x1.5f3a0cp-1f}, //     1  
  {{-0x1.886414p-1f,-0x1.886412p-1f}, 0x1.5f4a0cp-1f}, //     1  
  {{-0x1.8839b4p-1f,-0x1.8839b0p-1f}, 0x1.5f5a0ap-1f}, //     2  
  {{-0x1.880f44p-1f,-0x1.880f42p-1f}, 0x1.5f6a0ap-1f}, //     1  
  {{-0x1.87e4d4p-1f,-0x1.87e4d0p-1f}, 0x1.5f7a08p-1f}, //     2  
  {{-0x1.87ba54p-1f,-0x1.87ba50p-1f}, 0x1.5f8a08p-1f}, //     2  
  {{-0x1.878fd2p-1f,-0x1.878fcep-1f}, 0x1.5f9a06p-1f}, //     2  
  {{-0x1.876542p-1f,-0x1.876540p-1f}, 0x1.5faa06p-1f}, //     1  
  {{-0x1.873ab0p-1f,-0x1.873aacp-1f}, 0x1.5fba04p-1f}, //     2  
  {{-0x1.871010p-1f,-0x1.87100cp-1f}, 0x1.5fca04p-1f}, //     2  
  {{-0x1.86e56ep-1f,-0x1.86e56ap-1f}, 0x1.5fda02p-1f}, //     2  
  {{-0x1.86babcp-1f,-0x1.86babap-1f}, 0x1.5fea02p-1f}, //     1  
  {{-0x1.86900ap-1f,-0x1.869006p-1f}, 0x1.5ffa00p-1f}, //     2  
  {{-0x1.866548p-1f,-0x1.866544p-1f}, 0x1.600a00p-1f}, //     2  
  {{-0x1.863a84p-1f,-0x1.863a80p-1f}, 0x1.6019fep-1f}, //     2  
  {{-0x1.860fb2p-1f,-0x1.860fb0p-1f}, 0x1.6029fep-1f}, //     1  
  {{-0x1.85e4dep-1f,-0x1.85e4dap-1f}, 0x1.6039fcp-1f}, //     2  
  {{-0x1.85b9fcp-1f,-0x1.85b9f8p-1f}, 0x1.6049fcp-1f}, //     2  
  {{-0x1.858f18p-1f,-0x1.858f14p-1f}, 0x1.6059fap-1f}, //     2  
  {{-0x1.856424p-1f,-0x1.856422p-1f}, 0x1.6069fap-1f}, //     1  
  {{-0x1.853930p-1f,-0x1.85392cp-1f}, 0x1.6079f8p-1f}, //     2  
  {{-0x1.850e2cp-1f,-0x1.850e28p-1f}, 0x1.6089f8p-1f}, //     2  
  {{-0x1.84e326p-1f,-0x1.84e322p-1f}, 0x1.6099f6p-1f}, //     2  
  {{-0x1.84b812p-1f,-0x1.84b810p-1f}, 0x1.60a9f6p-1f}, //     1  
  {{-0x1.848cfcp-1f,-0x1.848cf8p-1f}, 0x1.60b9f4p-1f}, //     2  
  {{-0x1.8461d8p-1f,-0x1.8461d4p-1f}, 0x1.60c9f4p-1f}, //     2  
  {{-0x1.8436b2p-1f,-0x1.8436aep-1f}, 0x1.60d9f2p-1f}, //     2  
  {{-0x1.840b7cp-1f,-0x1.840b7ap-1f}, 0x1.60e9f2p-1f}, //     1  
  {{-0x1.83e044p-1f,-0x1.83e042p-1f}, 0x1.60f9f0p-1f}, //     1  
  {{-0x1.83b500p-1f,-0x1.83b4fcp-1f}, 0x1.6109f0p-1f}, //     2  
  {{-0x1.8389b8p-1f,-0x1.8389b4p-1f}, 0x1.6119eep-1f}, //     2  
  {{-0x1.835e62p-1f,-0x1.835e5ep-1f}, 0x1.6129eep-1f}, //     2  
  {{-0x1.83330ap-1f,-0x1.833306p-1f}, 0x1.6139ecp-1f}, //     2  
  {{-0x1.8307a4p-1f,-0x1.8307a0p-1f}, 0x1.6149ecp-1f}, //     2  
  {{-0x1.82dc3ap-1f,-0x1.82dc36p-1f}, 0x1.6159eap-1f}, //     2  
  {{-0x1.82b0c4p-1f,-0x1.82b0c0p-1f}, 0x1.6169eap-1f}, //     2  
  {{-0x1.82854ap-1f,-0x1.828546p-1f}, 0x1.6179e8p-1f}, //     2  
  {{-0x1.8259c2p-1f,-0x1.8259bep-1f}, 0x1.6189e8p-1f}, //     2  
  {{-0x1.822e38p-1f,-0x1.822e34p-1f}, 0x1.6199e6p-1f}, //     2  
  {{-0x1.8202a0p-1f,-0x1.82029cp-1f}, 0x1.61a9e6p-1f}, //     2  
  {{-0x1.81d704p-1f,-0x1.81d702p-1f}, 0x1.61b9e4p-1f}, //     1  
  {{-0x1.81ab5cp-1f,-0x1.81ab58p-1f}, 0x1.61c9e4p-1f}, //     2  
  {{-0x1.817fb2p-1f,-0x1.817faep-1f}, 0x1.61d9e2p-1f}, //     2  
  {{-0x1.8153f8p-1f,-0x1.8153f4p-1f}, 0x1.61e9e2p-1f}, //     2  
  {{-0x1.81283cp-1f,-0x1.812838p-1f}, 0x1.61f9e0p-1f}, //     2  
  {{-0x1.80fc72p-1f,-0x1.80fc6ep-1f}, 0x1.6209e0p-1f}, //     2  
  {{-0x1.80d0a6p-1f,-0x1.80d0a2p-1f}, 0x1.6219dep-1f}, //     2  
  {{-0x1.80a4ccp-1f,-0x1.80a4c8p-1f}, 0x1.6229dep-1f}, //     2  
  {{-0x1.8078eep-1f,-0x1.8078eap-1f}, 0x1.6239dcp-1f}, //     2  
  {{-0x1.804d04p-1f,-0x1.804d00p-1f}, 0x1.6249dcp-1f}, //     2  
  {{-0x1.802116p-1f,-0x1.802112p-1f}, 0x1.6259dap-1f}, //     2  
  {{-0x1.7ff51ap-1f,-0x1.7ff516p-1f}, 0x1.6269dap-1f}, //     2  
  {{-0x1.7fc91cp-1f,-0x1.7fc918p-1f}, 0x1.6279d8p-1f}, //     2  
  {{-0x1.7f9d10p-1f,-0x1.7f9d0cp-1f}, 0x1.6289d8p-1f}, //     2  
  {{-0x1.7f7100p-1f,-0x1.7f70fep-1f}, 0x1.6299d6p-1f}, //     1  
  {{-0x1.7f44e4p-1f,-0x1.7f44e0p-1f}, 0x1.62a9d6p-1f}, //     2  
  {{-0x1.7f18c4p-1f,-0x1.7f18c0p-1f}, 0x1.62b9d4p-1f}, //     2  
  {{-0x1.7eec98p-1f,-0x1.7eec94p-1f}, 0x1.62c9d4p-1f}, //     2  
  {{-0x1.7ec066p-1f,-0x1.7ec064p-1f}, 0x1.62d9d2p-1f}, //     1  
  {{-0x1.7e9428p-1f,-0x1.7e9426p-1f}, 0x1.62e9d2p-1f}, //     1  
  {{-0x1.7e67e8p-1f,-0x1.7e67e4p-1f}, 0x1.62f9d0p-1f}, //     2  
  {{-0x1.7e3b9ap-1f,-0x1.7e3b96p-1f}, 0x1.6309d0p-1f}, //     2  
  {{-0x1.7e0f48p-1f,-0x1.7e0f44p-1f}, 0x1.6319cep-1f}, //     2  
  {{-0x1.7de2e8p-1f,-0x1.7de2e6p-1f}, 0x1.6329cep-1f}, //     1  
  {{-0x1.7db686p-1f,-0x1.7db684p-1f}, 0x1.6339ccp-1f}, //     1  
  {{-0x1.7d8a16p-1f,-0x1.7d8a14p-1f}, 0x1.6349ccp-1f}, //     1  
  {{-0x1.7d5da4p-1f,-0x1.7d5da0p-1f}, 0x1.6359cap-1f}, //     2  
  {{-0x1.7d3124p-1f,-0x1.7d3120p-1f}, 0x1.6369cap-1f}, //     2  
  {{-0x1.7d04a0p-1f,-0x1.7d049cp-1f}, 0x1.6379c8p-1f}, //     2  
  {{-0x1.7cd810p-1f,-0x1.7cd80cp-1f}, 0x1.6389c8p-1f}, //     2  
  {{-0x1.7cab7cp-1f,-0x1.7cab78p-1f}, 0x1.6399c6p-1f}, //     2  
  {{-0x1.7c7edap-1f,-0x1.7c7ed6p-1f}, 0x1.63a9c6p-1f}, //     2  
  {{-0x1.7c5234p-1f,-0x1.7c5232p-1f}, 0x1.63b9c4p-1f}, //     1  
  {{-0x1.7c2582p-1f,-0x1.7c257ep-1f}, 0x1.63c9c4p-1f}, //     2  
  {{-0x1.7bf8ccp-1f,-0x1.7bf8cap-1f}, 0x1.63d9c2p-1f}, //     1  
  {{-0x1.7bcc0ap-1f,-0x1.7bcc06p-1f}, 0x1.63e9c2p-1f}, //     2  
  {{-0x1.7b9f44p-1f,-0x1.7b9f40p-1f}, 0x1.63f9c0p-1f}, //     2  
  {{-0x1.7b7270p-1f,-0x1.7b726cp-1f}, 0x1.6409c0p-1f}, //     2  
  {{-0x1.7b459ap-1f,-0x1.7b4596p-1f}, 0x1.6419bep-1f}, //     2  
  {{-0x1.7b18b4p-1f,-0x1.7b18b0p-1f}, 0x1.6429bep-1f}, //     2  
  {{-0x1.7aebcep-1f,-0x1.7aebcap-1f}, 0x1.6439bcp-1f}, //     2  
  {{-0x1.7abed8p-1f,-0x1.7abed4p-1f}, 0x1.6449bcp-1f}, //     2  
  {{-0x1.7a91e0p-1f,-0x1.7a91dcp-1f}, 0x1.6459bap-1f}, //     2  
  {{-0x1.7a64dap-1f,-0x1.7a64d6p-1f}, 0x1.6469bap-1f}, //     2  
  {{-0x1.7a37d2p-1f,-0x1.7a37cep-1f}, 0x1.6479b8p-1f}, //     2  
  {{-0x1.7a0abap-1f,-0x1.7a0ab6p-1f}, 0x1.6489b8p-1f}, //     2  
  {{-0x1.79dda2p-1f,-0x1.79dd9ep-1f}, 0x1.6499b6p-1f}, //     2  
  {{-0x1.79b07ap-1f,-0x1.79b076p-1f}, 0x1.64a9b6p-1f}, //     2  
  {{-0x1.798350p-1f,-0x1.79834cp-1f}, 0x1.64b9b4p-1f}, //     2  
  {{-0x1.795618p-1f,-0x1.795614p-1f}, 0x1.64c9b4p-1f}, //     2  
  {{-0x1.7928dcp-1f,-0x1.7928d8p-1f}, 0x1.64d9b2p-1f}, //     2  
  {{-0x1.78fb94p-1f,-0x1.78fb90p-1f}, 0x1.64e9b2p-1f}, //     2  
  {{-0x1.78ce48p-1f,-0x1.78ce44p-1f}, 0x1.64f9b0p-1f}, //     2  
  {{-0x1.78a0eep-1f,-0x1.78a0eap-1f}, 0x1.6509b0p-1f}, //     2  
  {{-0x1.787392p-1f,-0x1.78738ep-1f}, 0x1.6519aep-1f}, //     2  
  {{-0x1.784628p-1f,-0x1.784624p-1f}, 0x1.6529aep-1f}, //     2  
  {{-0x1.7818bcp-1f,-0x1.7818b8p-1f}, 0x1.6539acp-1f}, //     2  
  {{-0x1.77eb40p-1f,-0x1.77eb3cp-1f}, 0x1.6549acp-1f}, //     2  
  {{-0x1.77bdc2p-1f,-0x1.77bdbep-1f}, 0x1.6559aap-1f}, //     2  
  {{-0x1.779036p-1f,-0x1.779032p-1f}, 0x1.6569aap-1f}, //     2  
  {{-0x1.7762a8p-1f,-0x1.7762a4p-1f}, 0x1.6579a8p-1f}, //     2  
  {{-0x1.77350cp-1f,-0x1.773508p-1f}, 0x1.6589a8p-1f}, //     2  
  {{-0x1.77076cp-1f,-0x1.770768p-1f}, 0x1.6599a6p-1f}, //     2  
  {{-0x1.76d9bep-1f,-0x1.76d9bcp-1f}, 0x1.65a9a6p-1f}, //     1  
  {{-0x1.76ac10p-1f,-0x1.76ac0cp-1f}, 0x1.65b9a4p-1f}, //     2  
  {{-0x1.767e50p-1f,-0x1.767e4ep-1f}, 0x1.65c9a4p-1f}, //     1  
  {{-0x1.765090p-1f,-0x1.76508cp-1f}, 0x1.65d9a2p-1f}, //     2  
  {{-0x1.7622c2p-1f,-0x1.7622bep-1f}, 0x1.65e9a2p-1f}, //     2  
  {{-0x1.75f4f0p-1f,-0x1.75f4ecp-1f}, 0x1.65f9a0p-1f}, //     2  
  {{-0x1.75c710p-1f,-0x1.75c70cp-1f}, 0x1.6609a0p-1f}, //     2  
  {{-0x1.75992ep-1f,-0x1.75992ap-1f}, 0x1.66199ep-1f}, //     2  
  {{-0x1.756b3ep-1f,-0x1.756b3ap-1f}, 0x1.66299ep-1f}, //     2  
  {{-0x1.753d4ap-1f,-0x1.753d46p-1f}, 0x1.66399cp-1f}, //     2  
  {{-0x1.750f4ap-1f,-0x1.750f46p-1f}, 0x1.66499cp-1f}, //     2  
  {{-0x1.74e146p-1f,-0x1.74e142p-1f}, 0x1.66599ap-1f}, //     2  
  {{-0x1.74b334p-1f,-0x1.74b330p-1f}, 0x1.66699ap-1f}, //     2  
  {{-0x1.748520p-1f,-0x1.74851cp-1f}, 0x1.667998p-1f}, //     2  
  {{-0x1.7456fcp-1f,-0x1.7456f8p-1f}, 0x1.668998p-1f}, //     2  
  {{-0x1.7428d8p-1f,-0x1.7428d4p-1f}, 0x1.669996p-1f}, //     2  
  {{-0x1.73faa4p-1f,-0x1.73faa0p-1f}, 0x1.66a996p-1f}, //     2  
  {{-0x1.73cc6ep-1f,-0x1.73cc6ap-1f}, 0x1.66b994p-1f}, //     2  
  {{-0x1.739e28p-1f,-0x1.739e26p-1f}, 0x1.66c994p-1f}, //     1  
  {{-0x1.736fe2p-1f,-0x1.736fdep-1f}, 0x1.66d992p-1f}, //     2  
  {{-0x1.73418cp-1f,-0x1.734188p-1f}, 0x1.66e992p-1f}, //     2  
  {{-0x1.731334p-1f,-0x1.731332p-1f}, 0x1.66f990p-1f}, //     1  
  {{-0x1.72e4cep-1f,-0x1.72e4ccp-1f}, 0x1.670990p-1f}, //     1  
  {{-0x1.72b666p-1f,-0x1.72b662p-1f}, 0x1.67198ep-1f}, //     2  
  {{-0x1.7287f0p-1f,-0x1.7287ecp-1f}, 0x1.67298ep-1f}, //     2  
  {{-0x1.725976p-1f,-0x1.725972p-1f}, 0x1.67398cp-1f}, //     2  
  {{-0x1.722aeep-1f,-0x1.722aeap-1f}, 0x1.67498cp-1f}, //     2  
  {{-0x1.71fc64p-1f,-0x1.71fc60p-1f}, 0x1.67598ap-1f}, //     2  
  {{-0x1.71cdccp-1f,-0x1.71cdc8p-1f}, 0x1.67698ap-1f}, //     2  
  {{-0x1.719f30p-1f,-0x1.719f2cp-1f}, 0x1.677988p-1f}, //     2  
  {{-0x1.717088p-1f,-0x1.717084p-1f}, 0x1.678988p-1f}, //     2  
  {{-0x1.7141dcp-1f,-0x1.7141d8p-1f}, 0x1.679986p-1f}, //     2  
  {{-0x1.711322p-1f,-0x1.71131ep-1f}, 0x1.67a986p-1f}, //     2  
  {{-0x1.70e464p-1f,-0x1.70e460p-1f}, 0x1.67b984p-1f}, //     2  
  {{-0x1.70b59ap-1f,-0x1.70b596p-1f}, 0x1.67c984p-1f}, //     2  
  {{-0x1.7086ccp-1f,-0x1.7086c8p-1f}, 0x1.67d982p-1f}, //     2  
  {{-0x1.7057f0p-1f,-0x1.7057ecp-1f}, 0x1.67e982p-1f}, //     2  
  {{-0x1.702912p-1f,-0x1.70290ep-1f}, 0x1.67f980p-1f}, //     2  
  {{-0x1.6ffa24p-1f,-0x1.6ffa20p-1f}, 0x1.680980p-1f}, //     2  
  {{-0x1.6fcb34p-1f,-0x1.6fcb32p-1f}, 0x1.68197ep-1f}, //     1  
  {{-0x1.6f9c38p-1f,-0x1.6f9c34p-1f}, 0x1.68297ep-1f}, //     2  
  {{-0x1.6f6d38p-1f,-0x1.6f6d34p-1f}, 0x1.68397cp-1f}, //     2  
  {{-0x1.6f3e28p-1f,-0x1.6f3e24p-1f}, 0x1.68497cp-1f}, //     2  
  {{-0x1.6f0f18p-1f,-0x1.6f0f14p-1f}, 0x1.68597ap-1f}, //     2  
  {{-0x1.6edff8p-1f,-0x1.6edff4p-1f}, 0x1.68697ap-1f}, //     2  
  {{-0x1.6eb0d6p-1f,-0x1.6eb0d2p-1f}, 0x1.687978p-1f}, //     2  
  {{-0x1.6e81a6p-1f,-0x1.6e81a2p-1f}, 0x1.688978p-1f}, //     2  
  {{-0x1.6e5272p-1f,-0x1.6e526ep-1f}, 0x1.689976p-1f}, //     2  
  {{-0x1.6e2332p-1f,-0x1.6e232ep-1f}, 0x1.68a976p-1f}, //     2  
  {{-0x1.6df3eep-1f,-0x1.6df3eap-1f}, 0x1.68b974p-1f}, //     2  
  {{-0x1.6dc49cp-1f,-0x1.6dc498p-1f}, 0x1.68c974p-1f}, //     2  
  {{-0x1.6d9548p-1f,-0x1.6d9544p-1f}, 0x1.68d972p-1f}, //     2  
  {{-0x1.6d65e4p-1f,-0x1.6d65e0p-1f}, 0x1.68e972p-1f}, //     2  
  {{-0x1.6d367ep-1f,-0x1.6d367ap-1f}, 0x1.68f970p-1f}, //     2  
  {{-0x1.6d070ap-1f,-0x1.6d0706p-1f}, 0x1.690970p-1f}, //     2  
  {{-0x1.6cd794p-1f,-0x1.6cd790p-1f}, 0x1.69196ep-1f}, //     2  
  {{-0x1.6ca810p-1f,-0x1.6ca80cp-1f}, 0x1.69296ep-1f}, //     2  
  {{-0x1.6c7888p-1f,-0x1.6c7884p-1f}, 0x1.69396cp-1f}, //     2  
  {{-0x1.6c48f2p-1f,-0x1.6c48eep-1f}, 0x1.69496cp-1f}, //     2  
  {{-0x1.6c195ap-1f,-0x1.6c1956p-1f}, 0x1.69596ap-1f}, //     2  
  {{-0x1.6be9b4p-1f,-0x1.6be9b0p-1f}, 0x1.69696ap-1f}, //     2  
  {{-0x1.6bba0ap-1f,-0x1.6bba06p-1f}, 0x1.697968p-1f}, //     2  
  {{-0x1.6b8a52p-1f,-0x1.6b8a4ep-1f}, 0x1.698968p-1f}, //     2  
  {{-0x1.6b5a98p-1f,-0x1.6b5a94p-1f}, 0x1.699966p-1f}, //     2  
  {{-0x1.6b2ad0p-1f,-0x1.6b2accp-1f}, 0x1.69a966p-1f}, //     2  
  {{-0x1.6afb04p-1f,-0x1.6afb00p-1f}, 0x1.69b964p-1f}, //     2  
  {{-0x1.6acb2ap-1f,-0x1.6acb26p-1f}, 0x1.69c964p-1f}, //     2  
  {{-0x1.6a9b4ep-1f,-0x1.6a9b4ap-1f}, 0x1.69d962p-1f}, //     2  
  {{-0x1.6a6b64p-1f,-0x1.6a6b60p-1f}, 0x1.69e962p-1f}, //     2  
  {{-0x1.6a3b78p-1f,-0x1.6a3b74p-1f}, 0x1.69f960p-1f}, //     2  
  {{-0x1.6a0b7cp-1f,-0x1.6a0b78p-1f}, 0x1.6a0960p-1f}, //     2  
  {{-0x1.69db7ep-1f,-0x1.69db7ap-1f}, 0x1.6a195ep-1f}, //     2  
  {{-0x1.69ab72p-1f,-0x1.69ab6ep-1f}, 0x1.6a295ep-1f}, //     2  
  {{-0x1.697b62p-1f,-0x1.697b5ep-1f}, 0x1.6a395cp-1f}, //     2  
  {{-0x1.694b44p-1f,-0x1.694b40p-1f}, 0x1.6a495cp-1f}, //     2  
  {{-0x1.691b24p-1f,-0x1.691b20p-1f}, 0x1.6a595ap-1f}, //     2  
  {{-0x1.68eaf6p-1f,-0x1.68eaf2p-1f}, 0x1.6a695ap-1f}, //     2  
  {{-0x1.68bac6p-1f,-0x1.68bac2p-1f}, 0x1.6a7958p-1f}, //     2  
  {{-0x1.688a86p-1f,-0x1.688a82p-1f}, 0x1.6a8958p-1f}, //     2  
  {{-0x1.685a44p-1f,-0x1.685a40p-1f}, 0x1.6a9956p-1f}, //     2  
  {{-0x1.6829f4p-1f,-0x1.6829f0p-1f}, 0x1.6aa956p-1f}, //     2  
  {{-0x1.67f9a2p-1f,-0x1.67f99ep-1f}, 0x1.6ab954p-1f}, //     2  
  {{-0x1.67c940p-1f,-0x1.67c93cp-1f}, 0x1.6ac954p-1f}, //     2  
  {{-0x1.6798dcp-1f,-0x1.6798d8p-1f}, 0x1.6ad952p-1f}, //     2  
  {{-0x1.67686ap-1f,-0x1.676866p-1f}, 0x1.6ae952p-1f}, //     2  
  {{-0x1.6737f4p-1f,-0x1.6737f0p-1f}, 0x1.6af950p-1f}, //     2  
  {{-0x1.670772p-1f,-0x1.67076ep-1f}, 0x1.6b0950p-1f}, //     2  
  {{-0x1.66d6ecp-1f,-0x1.66d6e8p-1f}, 0x1.6b194ep-1f}, //     2  
  {{-0x1.66a658p-1f,-0x1.66a654p-1f}, 0x1.6b294ep-1f}, //     2  
  {{-0x1.6675c0p-1f,-0x1.6675bcp-1f}, 0x1.6b394cp-1f}, //     2  
  {{-0x1.66451cp-1f,-0x1.664516p-1f}, 0x1.6b494cp-1f}, //     3  
  {{-0x1.661474p-1f,-0x1.661470p-1f}, 0x1.6b594ap-1f}, //     2  
  {{-0x1.65e3bcp-1f,-0x1.65e3b8p-1f}, 0x1.6b694ap-1f}, //     2  
  {{-0x1.65b304p-1f,-0x1.65b300p-1f}, 0x1.6b7948p-1f}, //     2  
  {{-0x1.65823cp-1f,-0x1.658238p-1f}, 0x1.6b8948p-1f}, //     2  
  {{-0x1.655172p-1f,-0x1.65516ep-1f}, 0x1.6b9946p-1f}, //     2  
  {{-0x1.65209ap-1f,-0x1.652096p-1f}, 0x1.6ba946p-1f}, //     2  
  {{-0x1.64efc0p-1f,-0x1.64efbap-1f}, 0x1.6bb944p-1f}, //     3  
  {{-0x1.64bed6p-1f,-0x1.64bed2p-1f}, 0x1.6bc944p-1f}, //     2  
  {{-0x1.648deap-1f,-0x1.648de6p-1f}, 0x1.6bd942p-1f}, //     2  
  {{-0x1.645ceep-1f,-0x1.645ceap-1f}, 0x1.6be942p-1f}, //     2  
  {{-0x1.642bf2p-1f,-0x1.642beep-1f}, 0x1.6bf940p-1f}, //     2  
  {{-0x1.63fae6p-1f,-0x1.63fae2p-1f}, 0x1.6c0940p-1f}, //     2  
  {{-0x1.63c9d8p-1f,-0x1.63c9d4p-1f}, 0x1.6c193ep-1f}, //     2  
  {{-0x1.6398bcp-1f,-0x1.6398b8p-1f}, 0x1.6c293ep-1f}, //     2  
  {{-0x1.63679cp-1f,-0x1.636798p-1f}, 0x1.6c393cp-1f}, //     2  
  {{-0x1.63366ep-1f,-0x1.63366ap-1f}, 0x1.6c493cp-1f}, //     2  
  {{-0x1.63053ep-1f,-0x1.63053ap-1f}, 0x1.6c593ap-1f}, //     2  
  {{-0x1.62d400p-1f,-0x1.62d3fcp-1f}, 0x1.6c693ap-1f}, //     2  
  {{-0x1.62a2bep-1f,-0x1.62a2bap-1f}, 0x1.6c7938p-1f}, //     2  
  {{-0x1.62716ep-1f,-0x1.62716ap-1f}, 0x1.6c8938p-1f}, //     2  
  {{-0x1.62401cp-1f,-0x1.624018p-1f}, 0x1.6c9936p-1f}, //     2  
  {{-0x1.620ebcp-1f,-0x1.620eb6p-1f}, 0x1.6ca936p-1f}, //     3  
  {{-0x1.61dd58p-1f,-0x1.61dd54p-1f}, 0x1.6cb934p-1f}, //     2  
  {{-0x1.61abe6p-1f,-0x1.61abe2p-1f}, 0x1.6cc934p-1f}, //     2  
  {{-0x1.617a70p-1f,-0x1.617a6cp-1f}, 0x1.6cd932p-1f}, //     2  
  {{-0x1.6148eep-1f,-0x1.6148eap-1f}, 0x1.6ce932p-1f}, //     2  
  {{-0x1.611768p-1f,-0x1.611764p-1f}, 0x1.6cf930p-1f}, //     2  
  {{-0x1.60e5d4p-1f,-0x1.60e5d0p-1f}, 0x1.6d0930p-1f}, //     2  
  {{-0x1.60b43ep-1f,-0x1.60b43ap-1f}, 0x1.6d192ep-1f}, //     2  
  {{-0x1.608298p-1f,-0x1.608294p-1f}, 0x1.6d292ep-1f}, //     2  
  {{-0x1.6050f0p-1f,-0x1.6050ecp-1f}, 0x1.6d392cp-1f}, //     2  
  {{-0x1.601f3ap-1f,-0x1.601f36p-1f}, 0x1.6d492cp-1f}, //     2  
  {{-0x1.5fed80p-1f,-0x1.5fed7cp-1f}, 0x1.6d592ap-1f}, //     2  
  {{-0x1.5fbbb8p-1f,-0x1.5fbbb4p-1f}, 0x1.6d692ap-1f}, //     2  
  {{-0x1.5f89eep-1f,-0x1.5f89eap-1f}, 0x1.6d7928p-1f}, //     2  
  {{-0x1.5f5816p-1f,-0x1.5f5812p-1f}, 0x1.6d8928p-1f}, //     2  
  {{-0x1.5f263cp-1f,-0x1.5f2636p-1f}, 0x1.6d9926p-1f}, //     3  
  {{-0x1.5ef452p-1f,-0x1.5ef44cp-1f}, 0x1.6da926p-1f}, //     3  
  {{-0x1.5ec264p-1f,-0x1.5ec260p-1f}, 0x1.6db924p-1f}, //     2  
  {{-0x1.5e906ap-1f,-0x1.5e9066p-1f}, 0x1.6dc924p-1f}, //     2  
  {{-0x1.5e5e6cp-1f,-0x1.5e5e68p-1f}, 0x1.6dd922p-1f}, //     2  
  {{-0x1.5e2c60p-1f,-0x1.5e2c5cp-1f}, 0x1.6de922p-1f}, //     2  
  {{-0x1.5dfa52p-1f,-0x1.5dfa4ep-1f}, 0x1.6df920p-1f}, //     2  
  {{-0x1.5dc834p-1f,-0x1.5dc830p-1f}, 0x1.6e0920p-1f}, //     2  
  {{-0x1.5d9614p-1f,-0x1.5d9610p-1f}, 0x1.6e191ep-1f}, //     2  
  {{-0x1.5d63e6p-1f,-0x1.5d63e2p-1f}, 0x1.6e291ep-1f}, //     2  
  {{-0x1.5d31b6p-1f,-0x1.5d31b2p-1f}, 0x1.6e391cp-1f}, //     2  
  {{-0x1.5cff76p-1f,-0x1.5cff72p-1f}, 0x1.6e491cp-1f}, //     2  
  {{-0x1.5ccd34p-1f,-0x1.5ccd30p-1f}, 0x1.6e591ap-1f}, //     2  
  {{-0x1.5c9ae2p-1f,-0x1.5c9adep-1f}, 0x1.6e691ap-1f}, //     2  
  {{-0x1.5c6890p-1f,-0x1.5c688cp-1f}, 0x1.6e7918p-1f}, //     2  
  {{-0x1.5c362ep-1f,-0x1.5c362ap-1f}, 0x1.6e8918p-1f}, //     2  
  {{-0x1.5c03cap-1f,-0x1.5c03c6p-1f}, 0x1.6e9916p-1f}, //     2  
  {{-0x1.5bd156p-1f,-0x1.5bd152p-1f}, 0x1.6ea916p-1f}, //     2  
  {{-0x1.5b9ee0p-1f,-0x1.5b9edcp-1f}, 0x1.6eb914p-1f}, //     2  
  {{-0x1.5b6c5cp-1f,-0x1.5b6c58p-1f}, 0x1.6ec914p-1f}, //     2  
  {{-0x1.5b39d6p-1f,-0x1.5b39d2p-1f}, 0x1.6ed912p-1f}, //     2  
  {{-0x1.5b0740p-1f,-0x1.5b073cp-1f}, 0x1.6ee912p-1f}, //     2  
  {{-0x1.5ad4a8p-1f,-0x1.5ad4a4p-1f}, 0x1.6ef910p-1f}, //     2  
  {{-0x1.5aa202p-1f,-0x1.5aa1fep-1f}, 0x1.6f0910p-1f}, //     2  
  {{-0x1.5a6f58p-1f,-0x1.5a6f54p-1f}, 0x1.6f190ep-1f}, //     2  
  {{-0x1.5a3ca0p-1f,-0x1.5a3c9cp-1f}, 0x1.6f290ep-1f}, //     2  
  {{-0x1.5a09e6p-1f,-0x1.5a09e2p-1f}, 0x1.6f390cp-1f}, //     2  
  {{-0x1.59d71ep-1f,-0x1.59d718p-1f}, 0x1.6f490cp-1f}, //     3  
  {{-0x1.59a452p-1f,-0x1.59a44ep-1f}, 0x1.6f590ap-1f}, //     2  
  {{-0x1.597178p-1f,-0x1.597174p-1f}, 0x1.6f690ap-1f}, //     2  
  {{-0x1.593e9ap-1f,-0x1.593e96p-1f}, 0x1.6f7908p-1f}, //     2  
  {{-0x1.590bb0p-1f,-0x1.590baap-1f}, 0x1.6f8908p-1f}, //     3  
  {{-0x1.58d8c2p-1f,-0x1.58d8bep-1f}, 0x1.6f9906p-1f}, //     2  
  {{-0x1.58a5c4p-1f,-0x1.58a5c0p-1f}, 0x1.6fa906p-1f}, //     2  
  {{-0x1.5872c6p-1f,-0x1.5872c2p-1f}, 0x1.6fb904p-1f}, //     2  
  {{-0x1.583fb8p-1f,-0x1.583fb4p-1f}, 0x1.6fc904p-1f}, //     2  
  {{-0x1.580ca8p-1f,-0x1.580ca2p-1f}, 0x1.6fd902p-1f}, //     3  
  {{-0x1.57d988p-1f,-0x1.57d984p-1f}, 0x1.6fe902p-1f}, //     2  
  {{-0x1.57a666p-1f,-0x1.57a662p-1f}, 0x1.6ff900p-1f}, //     2  
  {{-0x1.577336p-1f,-0x1.577332p-1f}, 0x1.700900p-1f}, //     2  
  {{-0x1.574004p-1f,-0x1.573ffep-1f}, 0x1.7018fep-1f}, //     3  
  {{-0x1.570cc2p-1f,-0x1.570cbcp-1f}, 0x1.7028fep-1f}, //     3  
  {{-0x1.56d97ep-1f,-0x1.56d978p-1f}, 0x1.7038fcp-1f}, //     3  
  {{-0x1.56a62ap-1f,-0x1.56a626p-1f}, 0x1.7048fcp-1f}, //     2  
  {{-0x1.5672d4p-1f,-0x1.5672d0p-1f}, 0x1.7058fap-1f}, //     2  
  {{-0x1.563f70p-1f,-0x1.563f6cp-1f}, 0x1.7068fap-1f}, //     2  
  {{-0x1.560c0ap-1f,-0x1.560c06p-1f}, 0x1.7078f8p-1f}, //     2  
  {{-0x1.55d894p-1f,-0x1.55d890p-1f}, 0x1.7088f8p-1f}, //     2  
  {{-0x1.55a51cp-1f,-0x1.55a518p-1f}, 0x1.7098f6p-1f}, //     2  
  {{-0x1.557196p-1f,-0x1.557192p-1f}, 0x1.70a8f6p-1f}, //     2  
  {{-0x1.553e0cp-1f,-0x1.553e08p-1f}, 0x1.70b8f4p-1f}, //     2  
  {{-0x1.550a74p-1f,-0x1.550a70p-1f}, 0x1.70c8f4p-1f}, //     2  
  {{-0x1.54d6dap-1f,-0x1.54d6d6p-1f}, 0x1.70d8f2p-1f}, //     2  
  {{-0x1.54a332p-1f,-0x1.54a32cp-1f}, 0x1.70e8f2p-1f}, //     3  
  {{-0x1.546f86p-1f,-0x1.546f82p-1f}, 0x1.70f8f0p-1f}, //     2  
  {{-0x1.543bcap-1f,-0x1.543bc6p-1f}, 0x1.7108f0p-1f}, //     2  
  {{-0x1.54080ep-1f,-0x1.54080ap-1f}, 0x1.7118eep-1f}, //     2  
  {{-0x1.53d442p-1f,-0x1.53d43ep-1f}, 0x1.7128eep-1f}, //     2  
  {{-0x1.53a074p-1f,-0x1.53a070p-1f}, 0x1.7138ecp-1f}, //     2  
  {{-0x1.536c96p-1f,-0x1.536c92p-1f}, 0x1.7148ecp-1f}, //     2  
  {{-0x1.5338b8p-1f,-0x1.5338b2p-1f}, 0x1.7158eap-1f}, //     3  
  {{-0x1.5304c8p-1f,-0x1.5304c4p-1f}, 0x1.7168eap-1f}, //     2  
  {{-0x1.52d0d8p-1f,-0x1.52d0d4p-1f}, 0x1.7178e8p-1f}, //     2  
  {{-0x1.529cd8p-1f,-0x1.529cd4p-1f}, 0x1.7188e8p-1f}, //     2  
  {{-0x1.5268d6p-1f,-0x1.5268d2p-1f}, 0x1.7198e6p-1f}, //     2  
  {{-0x1.5234c4p-1f,-0x1.5234c0p-1f}, 0x1.71a8e6p-1f}, //     2  
  {{-0x1.5200b2p-1f,-0x1.5200acp-1f}, 0x1.71b8e4p-1f}, //     3  
  {{-0x1.51cc8ep-1f,-0x1.51cc8ap-1f}, 0x1.71c8e4p-1f}, //     2  
  {{-0x1.51986ap-1f,-0x1.519866p-1f}, 0x1.71d8e2p-1f}, //     2  
  {{-0x1.516436p-1f,-0x1.516432p-1f}, 0x1.71e8e2p-1f}, //     2  
  {{-0x1.513000p-1f,-0x1.512ffcp-1f}, 0x1.71f8e0p-1f}, //     2  
  {{-0x1.50fbbap-1f,-0x1.50fbb6p-1f}, 0x1.7208e0p-1f}, //     2  
  {{-0x1.50c774p-1f,-0x1.50c76ep-1f}, 0x1.7218dep-1f}, //     3  
  {{-0x1.50931cp-1f,-0x1.509318p-1f}, 0x1.7228dep-1f}, //     2  
  {{-0x1.505ec4p-1f,-0x1.505ec0p-1f}, 0x1.7238dcp-1f}, //     2  
  {{-0x1.502a5cp-1f,-0x1.502a58p-1f}, 0x1.7248dcp-1f}, //     2  
  {{-0x1.4ff5f2p-1f,-0x1.4ff5eep-1f}, 0x1.7258dap-1f}, //     2  
  {{-0x1.4fc178p-1f,-0x1.4fc174p-1f}, 0x1.7268dap-1f}, //     2  
  {{-0x1.4f8cfep-1f,-0x1.4f8cfap-1f}, 0x1.7278d8p-1f}, //     2  
  {{-0x1.4f5872p-1f,-0x1.4f586ep-1f}, 0x1.7288d8p-1f}, //     2  
  {{-0x1.4f23e6p-1f,-0x1.4f23e2p-1f}, 0x1.7298d6p-1f}, //     2  
  {{-0x1.4eef4ap-1f,-0x1.4eef46p-1f}, 0x1.72a8d6p-1f}, //     2  
  {{-0x1.4ebaacp-1f,-0x1.4ebaa8p-1f}, 0x1.72b8d4p-1f}, //     2  
  {{-0x1.4e85fep-1f,-0x1.4e85fap-1f}, 0x1.72c8d4p-1f}, //     2  
  {{-0x1.4e5150p-1f,-0x1.4e514ap-1f}, 0x1.72d8d2p-1f}, //     3  
  {{-0x1.4e1c90p-1f,-0x1.4e1c8cp-1f}, 0x1.72e8d2p-1f}, //     2  
  {{-0x1.4de7d0p-1f,-0x1.4de7ccp-1f}, 0x1.72f8d0p-1f}, //     2  
  {{-0x1.4db300p-1f,-0x1.4db2fap-1f}, 0x1.7308d0p-1f}, //     3  
  {{-0x1.4d7e2ep-1f,-0x1.4d7e28p-1f}, 0x1.7318cep-1f}, //     3  
  {{-0x1.4d494cp-1f,-0x1.4d4948p-1f}, 0x1.7328cep-1f}, //     2  
  {{-0x1.4d1468p-1f,-0x1.4d1464p-1f}, 0x1.7338ccp-1f}, //     2  
  {{-0x1.4cdf76p-1f,-0x1.4cdf70p-1f}, 0x1.7348ccp-1f}, //     3  
  {{-0x1.4caa80p-1f,-0x1.4caa7cp-1f}, 0x1.7358cap-1f}, //     2  
  {{-0x1.4c757cp-1f,-0x1.4c7578p-1f}, 0x1.7368cap-1f}, //     2  
  {{-0x1.4c4076p-1f,-0x1.4c4072p-1f}, 0x1.7378c8p-1f}, //     2  
  {{-0x1.4c0b60p-1f,-0x1.4c0b5cp-1f}, 0x1.7388c8p-1f}, //     2  
  {{-0x1.4bd648p-1f,-0x1.4bd644p-1f}, 0x1.7398c6p-1f}, //     2  
  {{-0x1.4ba122p-1f,-0x1.4ba11cp-1f}, 0x1.73a8c6p-1f}, //     3  
  {{-0x1.4b6bf8p-1f,-0x1.4b6bf4p-1f}, 0x1.73b8c4p-1f}, //     2  
  {{-0x1.4b36c0p-1f,-0x1.4b36bap-1f}, 0x1.73c8c4p-1f}, //     3  
  {{-0x1.4b0184p-1f,-0x1.4b0180p-1f}, 0x1.73d8c2p-1f}, //     2  
  {{-0x1.4acc3ap-1f,-0x1.4acc36p-1f}, 0x1.73e8c2p-1f}, //     2  
  {{-0x1.4a96eep-1f,-0x1.4a96eap-1f}, 0x1.73f8c0p-1f}, //     2  
  {{-0x1.4a6194p-1f,-0x1.4a618ep-1f}, 0x1.7408c0p-1f}, //     3  
  {{-0x1.4a2c36p-1f,-0x1.4a2c32p-1f}, 0x1.7418bep-1f}, //     2  
  {{-0x1.49f6c8p-1f,-0x1.49f6c4p-1f}, 0x1.7428bep-1f}, //     2  
  {{-0x1.49c15ap-1f,-0x1.49c156p-1f}, 0x1.7438bcp-1f}, //     2  
  {{-0x1.498bdcp-1f,-0x1.498bd8p-1f}, 0x1.7448bcp-1f}, //     2  
  {{-0x1.49565cp-1f,-0x1.495656p-1f}, 0x1.7458bap-1f}, //     3  
  {{-0x1.4920ccp-1f,-0x1.4920c8p-1f}, 0x1.7468bap-1f}, //     2  
  {{-0x1.48eb3ap-1f,-0x1.48eb36p-1f}, 0x1.7478b8p-1f}, //     2  
  {{-0x1.48b598p-1f,-0x1.48b594p-1f}, 0x1.7488b8p-1f}, //     2  
  {{-0x1.487ff6p-1f,-0x1.487ff0p-1f}, 0x1.7498b6p-1f}, //     3  
  {{-0x1.484a42p-1f,-0x1.484a3ep-1f}, 0x1.74a8b6p-1f}, //     2  
  {{-0x1.48148ep-1f,-0x1.48148ap-1f}, 0x1.74b8b4p-1f}, //     2  
  {{-0x1.47decap-1f,-0x1.47dec6p-1f}, 0x1.74c8b4p-1f}, //     2  
  {{-0x1.47a904p-1f,-0x1.47a900p-1f}, 0x1.74d8b2p-1f}, //     2  
  {{-0x1.47732ep-1f,-0x1.47732ap-1f}, 0x1.74e8b2p-1f}, //     2  
  {{-0x1.473d56p-1f,-0x1.473d52p-1f}, 0x1.74f8b0p-1f}, //     2  
  {{-0x1.470770p-1f,-0x1.47076cp-1f}, 0x1.7508b0p-1f}, //     2  
  {{-0x1.46d186p-1f,-0x1.46d182p-1f}, 0x1.7518aep-1f}, //     2  
  {{-0x1.469b8ep-1f,-0x1.469b8ap-1f}, 0x1.7528aep-1f}, //     2  
  {{-0x1.466594p-1f,-0x1.46658ep-1f}, 0x1.7538acp-1f}, //     3  
  {{-0x1.462f8ap-1f,-0x1.462f84p-1f}, 0x1.7548acp-1f}, //     3  
  {{-0x1.45f97ep-1f,-0x1.45f978p-1f}, 0x1.7558aap-1f}, //     3  
  {{-0x1.45c362p-1f,-0x1.45c35ep-1f}, 0x1.7568aap-1f}, //     2  
  {{-0x1.458d44p-1f,-0x1.458d40p-1f}, 0x1.7578a8p-1f}, //     2  
  {{-0x1.455718p-1f,-0x1.455712p-1f}, 0x1.7588a8p-1f}, //     3  
  {{-0x1.4520e8p-1f,-0x1.4520e4p-1f}, 0x1.7598a6p-1f}, //     2  
  {{-0x1.44eaaap-1f,-0x1.44eaa6p-1f}, 0x1.75a8a6p-1f}, //     2  
  {{-0x1.44b46ap-1f,-0x1.44b464p-1f}, 0x1.75b8a4p-1f}, //     3  
  {{-0x1.447e1ap-1f,-0x1.447e14p-1f}, 0x1.75c8a4p-1f}, //     3  
  {{-0x1.4447c8p-1f,-0x1.4447c2p-1f}, 0x1.75d8a2p-1f}, //     3  
  {{-0x1.441166p-1f,-0x1.441162p-1f}, 0x1.75e8a2p-1f}, //     2  
  {{-0x1.43db02p-1f,-0x1.43dafep-1f}, 0x1.75f8a0p-1f}, //     2  
  {{-0x1.43a490p-1f,-0x1.43a48ap-1f}, 0x1.7608a0p-1f}, //     3  
  {{-0x1.436e1ap-1f,-0x1.436e16p-1f}, 0x1.76189ep-1f}, //     2  
  {{-0x1.433796p-1f,-0x1.433790p-1f}, 0x1.76289ep-1f}, //     3  
  {{-0x1.430110p-1f,-0x1.43010ap-1f}, 0x1.76389cp-1f}, //     3  
  {{-0x1.42ca78p-1f,-0x1.42ca74p-1f}, 0x1.76489cp-1f}, //     2  
  {{-0x1.4293e0p-1f,-0x1.4293dcp-1f}, 0x1.76589ap-1f}, //     2  
  {{-0x1.425d3ap-1f,-0x1.425d34p-1f}, 0x1.76689ap-1f}, //     3  
  {{-0x1.422690p-1f,-0x1.42268ap-1f}, 0x1.767898p-1f}, //     3  
  {{-0x1.41efd6p-1f,-0x1.41efd2p-1f}, 0x1.768898p-1f}, //     2  
  {{-0x1.41b91cp-1f,-0x1.41b916p-1f}, 0x1.769896p-1f}, //     3  
  {{-0x1.418250p-1f,-0x1.41824cp-1f}, 0x1.76a896p-1f}, //     2  
  {{-0x1.414b84p-1f,-0x1.414b7ep-1f}, 0x1.76b894p-1f}, //     3  
  {{-0x1.4114a8p-1f,-0x1.4114a2p-1f}, 0x1.76c894p-1f}, //     3  
  {{-0x1.40ddc8p-1f,-0x1.40ddc4p-1f}, 0x1.76d892p-1f}, //     2  
  {{-0x1.40a6dap-1f,-0x1.40a6d6p-1f}, 0x1.76e892p-1f}, //     2  
  {{-0x1.406fecp-1f,-0x1.406fe6p-1f}, 0x1.76f890p-1f}, //     3  
  {{-0x1.4038ecp-1f,-0x1.4038e6p-1f}, 0x1.770890p-1f}, //     3  
  {{-0x1.4001eap-1f,-0x1.4001e6p-1f}, 0x1.77188ep-1f}, //     2  
  {{-0x1.3fcadap-1f,-0x1.3fcad4p-1f}, 0x1.77288ep-1f}, //     3  
  {{-0x1.3f93c6p-1f,-0x1.3f93c2p-1f}, 0x1.77388cp-1f}, //     2  
  {{-0x1.3f5ca4p-1f,-0x1.3f5c9ep-1f}, 0x1.77488cp-1f}, //     3  
  {{-0x1.3f257ep-1f,-0x1.3f257ap-1f}, 0x1.77588ap-1f}, //     2  
  {{-0x1.3eee4ap-1f,-0x1.3eee46p-1f}, 0x1.77688ap-1f}, //     2  
  {{-0x1.3eb714p-1f,-0x1.3eb710p-1f}, 0x1.777888p-1f}, //     2  
  {{-0x1.3e7fcep-1f,-0x1.3e7fcap-1f}, 0x1.778888p-1f}, //     2  
  {{-0x1.3e4888p-1f,-0x1.3e4882p-1f}, 0x1.779886p-1f}, //     3  
  {{-0x1.3e1130p-1f,-0x1.3e112ap-1f}, 0x1.77a886p-1f}, //     3  
  {{-0x1.3dd9d6p-1f,-0x1.3dd9d2p-1f}, 0x1.77b884p-1f}, //     2  
  {{-0x1.3da26ep-1f,-0x1.3da268p-1f}, 0x1.77c884p-1f}, //     3  
  {{-0x1.3d6b02p-1f,-0x1.3d6afep-1f}, 0x1.77d882p-1f}, //     2  
  {{-0x1.3d3388p-1f,-0x1.3d3382p-1f}, 0x1.77e882p-1f}, //     3  
  {{-0x1.3cfc0cp-1f,-0x1.3cfc06p-1f}, 0x1.77f880p-1f}, //     3  
  {{-0x1.3cc47ep-1f,-0x1.3cc47ap-1f}, 0x1.780880p-1f}, //     2  
  {{-0x1.3c8cf0p-1f,-0x1.3c8cecp-1f}, 0x1.78187ep-1f}, //     2  
  {{-0x1.3c5552p-1f,-0x1.3c554ep-1f}, 0x1.78287ep-1f}, //     2  
  {{-0x1.3c1db4p-1f,-0x1.3c1daep-1f}, 0x1.78387cp-1f}, //     3  
  {{-0x1.3be604p-1f,-0x1.3be5fep-1f}, 0x1.78487cp-1f}, //     3  
  {{-0x1.3bae52p-1f,-0x1.3bae4ep-1f}, 0x1.78587ap-1f}, //     2  
  {{-0x1.3b7690p-1f,-0x1.3b768cp-1f}, 0x1.78687ap-1f}, //     2  
  {{-0x1.3b3ecep-1f,-0x1.3b3ecap-1f}, 0x1.787878p-1f}, //     2  
  {{-0x1.3b06fcp-1f,-0x1.3b06f6p-1f}, 0x1.788878p-1f}, //     3  
  {{-0x1.3acf26p-1f,-0x1.3acf22p-1f}, 0x1.789876p-1f}, //     2  
  {{-0x1.3a9742p-1f,-0x1.3a973ep-1f}, 0x1.78a876p-1f}, //     2  
  {{-0x1.3a5f5cp-1f,-0x1.3a5f56p-1f}, 0x1.78b874p-1f}, //     3  
  {{-0x1.3a2766p-1f,-0x1.3a2760p-1f}, 0x1.78c874p-1f}, //     3  
  {{-0x1.39ef6ep-1f,-0x1.39ef68p-1f}, 0x1.78d872p-1f}, //     3  
  {{-0x1.39b766p-1f,-0x1.39b762p-1f}, 0x1.78e872p-1f}, //     2  
  {{-0x1.397f5cp-1f,-0x1.397f58p-1f}, 0x1.78f870p-1f}, //     2  
  {{-0x1.394742p-1f,-0x1.39473ep-1f}, 0x1.790870p-1f}, //     2  
  {{-0x1.390f28p-1f,-0x1.390f22p-1f}, 0x1.79186ep-1f}, //     3  
  {{-0x1.38d6fcp-1f,-0x1.38d6f8p-1f}, 0x1.79286ep-1f}, //     2  
  {{-0x1.389ed0p-1f,-0x1.389ecap-1f}, 0x1.79386cp-1f}, //     3  
  {{-0x1.386692p-1f,-0x1.38668ep-1f}, 0x1.79486cp-1f}, //     2  
  {{-0x1.382e54p-1f,-0x1.382e50p-1f}, 0x1.79586ap-1f}, //     2  
  {{-0x1.37f606p-1f,-0x1.37f600p-1f}, 0x1.79686ap-1f}, //     3  
  {{-0x1.37bdb6p-1f,-0x1.37bdb0p-1f}, 0x1.797868p-1f}, //     3  
  {{-0x1.378556p-1f,-0x1.378550p-1f}, 0x1.798868p-1f}, //     3  
  {{-0x1.374cf4p-1f,-0x1.374ceep-1f}, 0x1.799866p-1f}, //     3  
  {{-0x1.371482p-1f,-0x1.37147cp-1f}, 0x1.79a866p-1f}, //     3  
  {{-0x1.36dc0ep-1f,-0x1.36dc0ap-1f}, 0x1.79b864p-1f}, //     2  
  {{-0x1.36a38ap-1f,-0x1.36a386p-1f}, 0x1.79c864p-1f}, //     2  
  {{-0x1.366b06p-1f,-0x1.366b00p-1f}, 0x1.79d862p-1f}, //     3  
  {{-0x1.363270p-1f,-0x1.36326ap-1f}, 0x1.79e862p-1f}, //     3  
  {{-0x1.35f9dap-1f,-0x1.35f9d4p-1f}, 0x1.79f860p-1f}, //     3  
  {{-0x1.35c132p-1f,-0x1.35c12cp-1f}, 0x1.7a0860p-1f}, //     3  
  {{-0x1.35888ap-1f,-0x1.358884p-1f}, 0x1.7a185ep-1f}, //     3  
  {{-0x1.354fd0p-1f,-0x1.354fccp-1f}, 0x1.7a285ep-1f}, //     2  
  {{-0x1.351716p-1f,-0x1.351712p-1f}, 0x1.7a385cp-1f}, //     2  
  {{-0x1.34de4cp-1f,-0x1.34de46p-1f}, 0x1.7a485cp-1f}, //     3  
  {{-0x1.34a580p-1f,-0x1.34a57ap-1f}, 0x1.7a585ap-1f}, //     3  
  {{-0x1.346ca4p-1f,-0x1.346c9ep-1f}, 0x1.7a685ap-1f}, //     3  
  {{-0x1.3433c6p-1f,-0x1.3433c0p-1f}, 0x1.7a7858p-1f}, //     3  
  {{-0x1.33fad8p-1f,-0x1.33fad2p-1f}, 0x1.7a8858p-1f}, //     3  
  {{-0x1.33c1e8p-1f,-0x1.33c1e4p-1f}, 0x1.7a9856p-1f}, //     2  
  {{-0x1.3388e8p-1f,-0x1.3388e4p-1f}, 0x1.7aa856p-1f}, //     2  
  {{-0x1.334fe8p-1f,-0x1.334fe2p-1f}, 0x1.7ab854p-1f}, //     3  
  {{-0x1.3316d6p-1f,-0x1.3316d0p-1f}, 0x1.7ac854p-1f}, //     3  
  {{-0x1.32ddc2p-1f,-0x1.32ddbep-1f}, 0x1.7ad852p-1f}, //     2  
  {{-0x1.32a4a0p-1f,-0x1.32a49ap-1f}, 0x1.7ae852p-1f}, //     3  
  {{-0x1.326b7ap-1f,-0x1.326b76p-1f}, 0x1.7af850p-1f}, //     2  
  {{-0x1.323246p-1f,-0x1.323240p-1f}, 0x1.7b0850p-1f}, //     3  
  {{-0x1.31f910p-1f,-0x1.31f90ap-1f}, 0x1.7b184ep-1f}, //     3  
  {{-0x1.31bfc8p-1f,-0x1.31bfc4p-1f}, 0x1.7b284ep-1f}, //     2  
  {{-0x1.318680p-1f,-0x1.31867cp-1f}, 0x1.7b384cp-1f}, //     2  
  {{-0x1.314d28p-1f,-0x1.314d22p-1f}, 0x1.7b484cp-1f}, //     3  
  {{-0x1.3113cep-1f,-0x1.3113c8p-1f}, 0x1.7b584ap-1f}, //     3  
  {{-0x1.30da64p-1f,-0x1.30da5ep-1f}, 0x1.7b684ap-1f}, //     3  
  {{-0x1.30a0f8p-1f,-0x1.30a0f2p-1f}, 0x1.7b7848p-1f}, //     3  
  {{-0x1.30677cp-1f,-0x1.306776p-1f}, 0x1.7b8848p-1f}, //     3  
  {{-0x1.302dfep-1f,-0x1.302dfap-1f}, 0x1.7b9846p-1f}, //     2  
  {{-0x1.2ff470p-1f,-0x1.2ff46cp-1f}, 0x1.7ba846p-1f}, //     2  
  {{-0x1.2fbae2p-1f,-0x1.2fbadcp-1f}, 0x1.7bb844p-1f}, //     3  
  {{-0x1.2f8142p-1f,-0x1.2f813cp-1f}, 0x1.7bc844p-1f}, //     3  
  {{-0x1.2f47a0p-1f,-0x1.2f479cp-1f}, 0x1.7bd842p-1f}, //     2  
  {{-0x1.2f0df0p-1f,-0x1.2f0deap-1f}, 0x1.7be842p-1f}, //     3  
  {{-0x1.2ed43cp-1f,-0x1.2ed436p-1f}, 0x1.7bf840p-1f}, //     3  
  {{-0x1.2e9a78p-1f,-0x1.2e9a74p-1f}, 0x1.7c0840p-1f}, //     2  
  {{-0x1.2e60b4p-1f,-0x1.2e60aep-1f}, 0x1.7c183ep-1f}, //     3  
  {{-0x1.2e26e0p-1f,-0x1.2e26dap-1f}, 0x1.7c283ep-1f}, //     3  
  {{-0x1.2ded08p-1f,-0x1.2ded04p-1f}, 0x1.7c383cp-1f}, //     2  
  {{-0x1.2db322p-1f,-0x1.2db31cp-1f}, 0x1.7c483cp-1f}, //     3  
  {{-0x1.2d793ap-1f,-0x1.2d7934p-1f}, 0x1.7c583ap-1f}, //     3  
  {{-0x1.2d3f40p-1f,-0x1.2d3f3cp-1f}, 0x1.7c683ap-1f}, //     2  
  {{-0x1.2d0546p-1f,-0x1.2d0542p-1f}, 0x1.7c7838p-1f}, //     2  
  {{-0x1.2ccb3cp-1f,-0x1.2ccb36p-1f}, 0x1.7c8838p-1f}, //     3  
  {{-0x1.2c9130p-1f,-0x1.2c912ap-1f}, 0x1.7c9836p-1f}, //     3  
  {{-0x1.2c5714p-1f,-0x1.2c570ep-1f}, 0x1.7ca836p-1f}, //     3  
  {{-0x1.2c1cf6p-1f,-0x1.2c1cf0p-1f}, 0x1.7cb834p-1f}, //     3  
  {{-0x1.2be2c8p-1f,-0x1.2be2c2p-1f}, 0x1.7cc834p-1f}, //     3  
  {{-0x1.2ba898p-1f,-0x1.2ba892p-1f}, 0x1.7cd832p-1f}, //     3  
  {{-0x1.2b6e58p-1f,-0x1.2b6e52p-1f}, 0x1.7ce832p-1f}, //     3  
  {{-0x1.2b3416p-1f,-0x1.2b3412p-1f}, 0x1.7cf830p-1f}, //     2  
  {{-0x1.2af9c4p-1f,-0x1.2af9c0p-1f}, 0x1.7d0830p-1f}, //     2  
  {{-0x1.2abf72p-1f,-0x1.2abf6cp-1f}, 0x1.7d182ep-1f}, //     3  
  {{-0x1.2a850ep-1f,-0x1.2a8508p-1f}, 0x1.7d282ep-1f}, //     3  
  {{-0x1.2a4aa8p-1f,-0x1.2a4aa4p-1f}, 0x1.7d382cp-1f}, //     2  
  {{-0x1.2a1032p-1f,-0x1.2a102ep-1f}, 0x1.7d482cp-1f}, //     2  
  {{-0x1.29d5bcp-1f,-0x1.29d5b6p-1f}, 0x1.7d582ap-1f}, //     3  
  {{-0x1.299b34p-1f,-0x1.299b2ep-1f}, 0x1.7d682ap-1f}, //     3  
  {{-0x1.2960acp-1f,-0x1.2960a6p-1f}, 0x1.7d7828p-1f}, //     3  
  {{-0x1.292612p-1f,-0x1.29260cp-1f}, 0x1.7d8828p-1f}, //     3  
  {{-0x1.28eb78p-1f,-0x1.28eb72p-1f}, 0x1.7d9826p-1f}, //     3  
  {{-0x1.28b0ccp-1f,-0x1.28b0c6p-1f}, 0x1.7da826p-1f}, //     3  
  {{-0x1.287620p-1f,-0x1.28761ap-1f}, 0x1.7db824p-1f}, //     3  
  {{-0x1.283b62p-1f,-0x1.283b5cp-1f}, 0x1.7dc824p-1f}, //     3  
  {{-0x1.2800a4p-1f,-0x1.28009ep-1f}, 0x1.7dd822p-1f}, //     3  
  {{-0x1.27c5d4p-1f,-0x1.27c5d0p-1f}, 0x1.7de822p-1f}, //     2  
  {{-0x1.278b04p-1f,-0x1.278afep-1f}, 0x1.7df820p-1f}, //     3  
  {{-0x1.275024p-1f,-0x1.27501ep-1f}, 0x1.7e0820p-1f}, //     3  
  {{-0x1.271540p-1f,-0x1.27153cp-1f}, 0x1.7e181ep-1f}, //     2  
  {{-0x1.26da4ep-1f,-0x1.26da48p-1f}, 0x1.7e281ep-1f}, //     3  
  {{-0x1.269f5ap-1f,-0x1.269f54p-1f}, 0x1.7e381cp-1f}, //     3  
  {{-0x1.266456p-1f,-0x1.266450p-1f}, 0x1.7e481cp-1f}, //     3  
  {{-0x1.262950p-1f,-0x1.26294ap-1f}, 0x1.7e581ap-1f}, //     3  
  {{-0x1.25ee38p-1f,-0x1.25ee34p-1f}, 0x1.7e681ap-1f}, //     2  
  {{-0x1.25b320p-1f,-0x1.25b31cp-1f}, 0x1.7e7818p-1f}, //     2  
  {{-0x1.2577f8p-1f,-0x1.2577f2p-1f}, 0x1.7e8818p-1f}, //     3  
  {{-0x1.253ccep-1f,-0x1.253cc8p-1f}, 0x1.7e9816p-1f}, //     3  
  {{-0x1.250194p-1f,-0x1.25018ep-1f}, 0x1.7ea816p-1f}, //     3  
  {{-0x1.24c658p-1f,-0x1.24c652p-1f}, 0x1.7eb814p-1f}, //     3  
  {{-0x1.248b0cp-1f,-0x1.248b06p-1f}, 0x1.7ec814p-1f}, //     3  
  {{-0x1.244fbep-1f,-0x1.244fb8p-1f}, 0x1.7ed812p-1f}, //     3  
  {{-0x1.241460p-1f,-0x1.24145ap-1f}, 0x1.7ee812p-1f}, //     3  
  {{-0x1.23d900p-1f,-0x1.23d8fap-1f}, 0x1.7ef810p-1f}, //     3  
  {{-0x1.239d90p-1f,-0x1.239d8ap-1f}, 0x1.7f0810p-1f}, //     3  
  {{-0x1.23621ep-1f,-0x1.236218p-1f}, 0x1.7f180ep-1f}, //     3  
  {{-0x1.23269cp-1f,-0x1.232696p-1f}, 0x1.7f280ep-1f}, //     3  
  {{-0x1.22eb18p-1f,-0x1.22eb12p-1f}, 0x1.7f380cp-1f}, //     3  
  {{-0x1.22af84p-1f,-0x1.22af7ep-1f}, 0x1.7f480cp-1f}, //     3  
  {{-0x1.2273eep-1f,-0x1.2273e8p-1f}, 0x1.7f580ap-1f}, //     3  
  {{-0x1.223848p-1f,-0x1.223842p-1f}, 0x1.7f680ap-1f}, //     3  
  {{-0x1.21fca0p-1f,-0x1.21fc9ap-1f}, 0x1.7f7808p-1f}, //     3  
  {{-0x1.21c0e8p-1f,-0x1.21c0e2p-1f}, 0x1.7f8808p-1f}, //     3  
  {{-0x1.21852ep-1f,-0x1.218528p-1f}, 0x1.7f9806p-1f}, //     3  
  {{-0x1.214964p-1f,-0x1.21495ep-1f}, 0x1.7fa806p-1f}, //     3  
  {{-0x1.210d98p-1f,-0x1.210d94p-1f}, 0x1.7fb804p-1f}, //     2  
  {{-0x1.20d1bcp-1f,-0x1.20d1b8p-1f}, 0x1.7fc804p-1f}, //     2  
  {{-0x1.2095e0p-1f,-0x1.2095dap-1f}, 0x1.7fd802p-1f}, //     3  
  {{-0x1.2059f2p-1f,-0x1.2059ecp-1f}, 0x1.7fe802p-1f}, //     3  
  {{-0x1.201e02p-1f,-0x1.201dfcp-1f}, 0x1.7ff800p-1f}, //     3  
  {{-0x1.1fe202p-1f,-0x1.1fe1fcp-1f}, 0x1.800800p-1f}, //     3  
  {{-0x1.1fa5f8p-1f,-0x1.1fa5f4p-1f}, 0x1.801800p-1f}, //     2  
  {{-0x1.1f69eep-1f,-0x1.1f69e8p-1f}, 0x1.8027fep-1f}, //     3  
  {{-0x1.1f2ddap-1f,-0x1.1f2dd6p-1f}, 0x1.8037fcp-1f}, //     2  
  {{-0x1.1ef1b6p-1f,-0x1.1ef1b2p-1f}, 0x1.8047fcp-1f}, //     2  
  {{-0x1.1eb58ap-1f,-0x1.1eb584p-1f}, 0x1.8057fcp-1f}, //     3  
  {{-0x1.1e795cp-1f,-0x1.1e7956p-1f}, 0x1.8067fap-1f}, //     3  
  {{-0x1.1e3d24p-1f,-0x1.1e3d1ep-1f}, 0x1.8077f8p-1f}, //     3  
  {{-0x1.1e00dcp-1f,-0x1.1e00d6p-1f}, 0x1.8087f8p-1f}, //     3  
  {{-0x1.1dc48ap-1f,-0x1.1dc486p-1f}, 0x1.8097f8p-1f}, //     2  
  {{-0x1.1d8838p-1f,-0x1.1d8832p-1f}, 0x1.80a7f6p-1f}, //     3  
  {{-0x1.1d4bdcp-1f,-0x1.1d4bd8p-1f}, 0x1.80b7f4p-1f}, //     2  
  {{-0x1.1d0f70p-1f,-0x1.1d0f6ap-1f}, 0x1.80c7f4p-1f}, //     3  
  {{-0x1.1cd2fcp-1f,-0x1.1cd2f6p-1f}, 0x1.80d7f4p-1f}, //     3  
  {{-0x1.1c9684p-1f,-0x1.1c9680p-1f}, 0x1.80e7f2p-1f}, //     2  
  {{-0x1.1c5a06p-1f,-0x1.1c5a00p-1f}, 0x1.80f7f0p-1f}, //     3  
  {{-0x1.1c1d76p-1f,-0x1.1c1d70p-1f}, 0x1.8107f0p-1f}, //     3  
  {{-0x1.1be0dcp-1f,-0x1.1be0d6p-1f}, 0x1.8117f0p-1f}, //     3  
  {{-0x1.1ba442p-1f,-0x1.1ba43cp-1f}, 0x1.8127eep-1f}, //     3  
  {{-0x1.1b679ep-1f,-0x1.1b6798p-1f}, 0x1.8137ecp-1f}, //     3  
  {{-0x1.1b2aeap-1f,-0x1.1b2ae4p-1f}, 0x1.8147ecp-1f}, //     3  
  {{-0x1.1aee2cp-1f,-0x1.1aee26p-1f}, 0x1.8157ecp-1f}, //     3  
  {{-0x1.1ab16ep-1f,-0x1.1ab168p-1f}, 0x1.8167eap-1f}, //     3  
  {{-0x1.1a74a6p-1f,-0x1.1a74a0p-1f}, 0x1.8177e8p-1f}, //     3  
  {{-0x1.1a37cep-1f,-0x1.1a37c8p-1f}, 0x1.8187e8p-1f}, //     3  
  {{-0x1.19faecp-1f,-0x1.19fae6p-1f}, 0x1.8197e8p-1f}, //     3  
  {{-0x1.19be0ap-1f,-0x1.19be04p-1f}, 0x1.81a7e6p-1f}, //     3  
  {{-0x1.19811ep-1f,-0x1.198118p-1f}, 0x1.81b7e4p-1f}, //     3  
  {{-0x1.194420p-1f,-0x1.19441cp-1f}, 0x1.81c7e4p-1f}, //     2  
  {{-0x1.19071cp-1f,-0x1.190716p-1f}, 0x1.81d7e4p-1f}, //     3  
  {{-0x1.18ca14p-1f,-0x1.18ca0ep-1f}, 0x1.81e7e2p-1f}, //     3  
  {{-0x1.188d04p-1f,-0x1.188cfep-1f}, 0x1.81f7e0p-1f}, //     3  
  {{-0x1.184fe4p-1f,-0x1.184fdep-1f}, 0x1.8207e0p-1f}, //     3  
  {{-0x1.1812bap-1f,-0x1.1812b4p-1f}, 0x1.8217e0p-1f}, //     3  
  {{-0x1.17d58ep-1f,-0x1.17d58ap-1f}, 0x1.8227dep-1f}, //     2  
  {{-0x1.17985ap-1f,-0x1.179854p-1f}, 0x1.8237dcp-1f}, //     3  
  {{-0x1.175b16p-1f,-0x1.175b10p-1f}, 0x1.8247dcp-1f}, //     3  
  {{-0x1.171dc8p-1f,-0x1.171dc2p-1f}, 0x1.8257dcp-1f}, //     3  
  {{-0x1.16e078p-1f,-0x1.16e072p-1f}, 0x1.8267dap-1f}, //     3  
  {{-0x1.16a320p-1f,-0x1.16a31ap-1f}, 0x1.8277d8p-1f}, //     3  
  {{-0x1.1665b8p-1f,-0x1.1665b2p-1f}, 0x1.8287d8p-1f}, //     3  
  {{-0x1.162844p-1f,-0x1.162840p-1f}, 0x1.8297d8p-1f}, //     2  
  {{-0x1.15ead2p-1f,-0x1.15eaccp-1f}, 0x1.82a7d6p-1f}, //     3  
  {{-0x1.15ad54p-1f,-0x1.15ad50p-1f}, 0x1.82b7d4p-1f}, //     2  
  {{-0x1.156fc8p-1f,-0x1.156fc2p-1f}, 0x1.82c7d4p-1f}, //     3  
  {{-0x1.153232p-1f,-0x1.15322cp-1f}, 0x1.82d7d4p-1f}, //     3  
  {{-0x1.14f49ap-1f,-0x1.14f494p-1f}, 0x1.82e7d2p-1f}, //     3  
  {{-0x1.14b6f8p-1f,-0x1.14b6f2p-1f}, 0x1.82f7d0p-1f}, //     3  
  {{-0x1.147948p-1f,-0x1.147942p-1f}, 0x1.8307d0p-1f}, //     3  
  {{-0x1.143b8cp-1f,-0x1.143b86p-1f}, 0x1.8317d0p-1f}, //     3  
  {{-0x1.13fdd0p-1f,-0x1.13fdcap-1f}, 0x1.8327cep-1f}, //     3  
  {{-0x1.13c00cp-1f,-0x1.13c006p-1f}, 0x1.8337ccp-1f}, //     3  
  {{-0x1.138236p-1f,-0x1.138230p-1f}, 0x1.8347ccp-1f}, //     3  
  {{-0x1.134456p-1f,-0x1.134450p-1f}, 0x1.8357ccp-1f}, //     3  
  {{-0x1.130676p-1f,-0x1.130670p-1f}, 0x1.8367cap-1f}, //     3  
  {{-0x1.12c88cp-1f,-0x1.12c888p-1f}, 0x1.8377c8p-1f}, //     2  
  {{-0x1.128a92p-1f,-0x1.128a8cp-1f}, 0x1.8387c8p-1f}, //     3  
  {{-0x1.124c90p-1f,-0x1.124c8ap-1f}, 0x1.8397c8p-1f}, //     3  
  {{-0x1.120e8ap-1f,-0x1.120e86p-1f}, 0x1.83a7c6p-1f}, //     2  
  {{-0x1.11d07ep-1f,-0x1.11d078p-1f}, 0x1.83b7c4p-1f}, //     3  
  {{-0x1.11925ep-1f,-0x1.119258p-1f}, 0x1.83c7c4p-1f}, //     3  
  {{-0x1.115438p-1f,-0x1.115432p-1f}, 0x1.83d7c4p-1f}, //     3  
  {{-0x1.11160ep-1f,-0x1.111608p-1f}, 0x1.83e7c2p-1f}, //     3  
  {{-0x1.10d7dcp-1f,-0x1.10d7d6p-1f}, 0x1.83f7c0p-1f}, //     3  
  {{-0x1.10999ap-1f,-0x1.109994p-1f}, 0x1.8407c0p-1f}, //     3  
  {{-0x1.105b4ep-1f,-0x1.105b48p-1f}, 0x1.8417c0p-1f}, //     3  
  {{-0x1.101d00p-1f,-0x1.101cfap-1f}, 0x1.8427bep-1f}, //     3  
  {{-0x1.0fdeaap-1f,-0x1.0fdea4p-1f}, 0x1.8437bcp-1f}, //     3  
  {{-0x1.0fa042p-1f,-0x1.0fa03cp-1f}, 0x1.8447bcp-1f}, //     3  
  {{-0x1.0f61d2p-1f,-0x1.0f61ccp-1f}, 0x1.8457bcp-1f}, //     3  
  {{-0x1.0f2360p-1f,-0x1.0f235ap-1f}, 0x1.8467bap-1f}, //     3  
  {{-0x1.0ee4e6p-1f,-0x1.0ee4e0p-1f}, 0x1.8477b8p-1f}, //     3  
  {{-0x1.0ea65ap-1f,-0x1.0ea654p-1f}, 0x1.8487b8p-1f}, //     3  
  {{-0x1.0e67c6p-1f,-0x1.0e67c0p-1f}, 0x1.8497b8p-1f}, //     3  
  {{-0x1.0e2930p-1f,-0x1.0e292ap-1f}, 0x1.84a7b6p-1f}, //     3  
  {{-0x1.0dea90p-1f,-0x1.0dea8ap-1f}, 0x1.84b7b4p-1f}, //     3  
  {{-0x1.0dabe0p-1f,-0x1.0dabdap-1f}, 0x1.84c7b4p-1f}, //     3  
  {{-0x1.0d6d28p-1f,-0x1.0d6d22p-1f}, 0x1.84d7b4p-1f}, //     3  
  {{-0x1.0d2e6cp-1f,-0x1.0d2e66p-1f}, 0x1.84e7b2p-1f}, //     3  
  {{-0x1.0cefaap-1f,-0x1.0cefa4p-1f}, 0x1.84f7b0p-1f}, //     3  
  {{-0x1.0cb0d4p-1f,-0x1.0cb0cep-1f}, 0x1.8507b0p-1f}, //     3  
  {{-0x1.0c71f8p-1f,-0x1.0c71f2p-1f}, 0x1.8517b0p-1f}, //     3  
  {{-0x1.0c3318p-1f,-0x1.0c3312p-1f}, 0x1.8527aep-1f}, //     3  
  {{-0x1.0bf430p-1f,-0x1.0bf42ap-1f}, 0x1.8537acp-1f}, //     3  
  {{-0x1.0bb538p-1f,-0x1.0bb532p-1f}, 0x1.8547acp-1f}, //     3  
  {{-0x1.0b7636p-1f,-0x1.0b7630p-1f}, 0x1.8557acp-1f}, //     3  
  {{-0x1.0b3732p-1f,-0x1.0b372cp-1f}, 0x1.8567aap-1f}, //     3  
  {{-0x1.0af826p-1f,-0x1.0af820p-1f}, 0x1.8577a8p-1f}, //     3  
  {{-0x1.0ab908p-1f,-0x1.0ab902p-1f}, 0x1.8587a8p-1f}, //     3  
  {{-0x1.0a79e2p-1f,-0x1.0a79dcp-1f}, 0x1.8597a8p-1f}, //     3  
  {{-0x1.0a3abap-1f,-0x1.0a3ab4p-1f}, 0x1.85a7a6p-1f}, //     3  
  {{-0x1.09fb88p-1f,-0x1.09fb82p-1f}, 0x1.85b7a4p-1f}, //     3  
  {{-0x1.09bc46p-1f,-0x1.09bc40p-1f}, 0x1.85c7a4p-1f}, //     3  
  {{-0x1.097cfcp-1f,-0x1.097cf6p-1f}, 0x1.85d7a4p-1f}, //     3  
  {{-0x1.093daep-1f,-0x1.093daap-1f}, 0x1.85e7a2p-1f}, //     2  
  {{-0x1.08fe5ap-1f,-0x1.08fe54p-1f}, 0x1.85f7a0p-1f}, //     3  
  {{-0x1.08bef2p-1f,-0x1.08beeep-1f}, 0x1.8607a0p-1f}, //     2  
  {{-0x1.087f84p-1f,-0x1.087f7ep-1f}, 0x1.8617a0p-1f}, //     3  
  {{-0x1.084012p-1f,-0x1.08400cp-1f}, 0x1.86279ep-1f}, //     3  
  {{-0x1.080098p-1f,-0x1.080092p-1f}, 0x1.86379cp-1f}, //     3  
  {{-0x1.07c10ep-1f,-0x1.07c108p-1f}, 0x1.86479cp-1f}, //     3  
  {{-0x1.07817ap-1f,-0x1.078174p-1f}, 0x1.86579cp-1f}, //     3  
  {{-0x1.0741e4p-1f,-0x1.0741dep-1f}, 0x1.86679ap-1f}, //     3  
  {{-0x1.070246p-1f,-0x1.070240p-1f}, 0x1.867798p-1f}, //     3  
  {{-0x1.06c296p-1f,-0x1.06c290p-1f}, 0x1.868798p-1f}, //     3  
  {{-0x1.0682dcp-1f,-0x1.0682d6p-1f}, 0x1.869798p-1f}, //     3  
  {{-0x1.064322p-1f,-0x1.06431cp-1f}, 0x1.86a796p-1f}, //     3  
  {{-0x1.060360p-1f,-0x1.06035ap-1f}, 0x1.86b794p-1f}, //     3  
  {{-0x1.05c38ap-1f,-0x1.05c386p-1f}, 0x1.86c794p-1f}, //     2  
  {{-0x1.0583aep-1f,-0x1.0583a8p-1f}, 0x1.86d794p-1f}, //     3  
  {{-0x1.0543cep-1f,-0x1.0543c8p-1f}, 0x1.86e792p-1f}, //     3  
  {{-0x1.0503e8p-1f,-0x1.0503e2p-1f}, 0x1.86f790p-1f}, //     3  
  {{-0x1.04c3eep-1f,-0x1.04c3e8p-1f}, 0x1.870790p-1f}, //     3  
  {{-0x1.0483ecp-1f,-0x1.0483e6p-1f}, 0x1.871790p-1f}, //     3  
  {{-0x1.0443e8p-1f,-0x1.0443e2p-1f}, 0x1.87278ep-1f}, //     3  
  {{-0x1.0403dcp-1f,-0x1.0403d6p-1f}, 0x1.87378cp-1f}, //     3  
  {{-0x1.03c3bep-1f,-0x1.03c3b8p-1f}, 0x1.87478cp-1f}, //     3  
  {{-0x1.038398p-1f,-0x1.038392p-1f}, 0x1.87578cp-1f}, //     3  
  {{-0x1.034370p-1f,-0x1.03436ap-1f}, 0x1.87678ap-1f}, //     3  
  {{-0x1.030340p-1f,-0x1.03033ap-1f}, 0x1.877788p-1f}, //     3  
  {{-0x1.02c2fcp-1f,-0x1.02c2f6p-1f}, 0x1.878788p-1f}, //     3  
  {{-0x1.0282b2p-1f,-0x1.0282acp-1f}, 0x1.879788p-1f}, //     3  
  {{-0x1.024264p-1f,-0x1.02425ep-1f}, 0x1.87a786p-1f}, //     3  
  {{-0x1.020210p-1f,-0x1.020208p-1f}, 0x1.87b784p-1f}, //     4  
  {{-0x1.01c1a8p-1f,-0x1.01c1a2p-1f}, 0x1.87c784p-1f}, //     3  
  {{-0x1.018138p-1f,-0x1.018132p-1f}, 0x1.87d784p-1f}, //     3  
  {{-0x1.0140c6p-1f,-0x1.0140c0p-1f}, 0x1.87e782p-1f}, //     3  
  {{-0x1.01004cp-1f,-0x1.010046p-1f}, 0x1.87f780p-1f}, //     3  
  {{-0x1.00bfc0p-1f,-0x1.00bfbap-1f}, 0x1.880780p-1f}, //     3  
  {{-0x1.007f2cp-1f,-0x1.007f26p-1f}, 0x1.881780p-1f}, //     3  
  {{-0x1.003e96p-1f,-0x1.003e90p-1f}, 0x1.88277ep-1f}, //     3  
  {{-0x1.fffbeep-2f,-0x1.fffbe0p-2f}, 0x1.88377cp-1f}, //     7  
  {{-0x1.ff7a8cp-2f,-0x1.ff7a7ep-2f}, 0x1.88477cp-1f}, //     7  
  {{-0x1.fef91ap-2f,-0x1.fef90cp-2f}, 0x1.88577cp-1f}, //     7  
  {{-0x1.fe77a4p-2f,-0x1.fe7796p-2f}, 0x1.88677ap-1f}, //     7  
  {{-0x1.fdf61cp-2f,-0x1.fdf60ep-2f}, 0x1.887778p-1f}, //     7  
  {{-0x1.fd7472p-2f,-0x1.fd7464p-2f}, 0x1.888778p-1f}, //     7  
  {{-0x1.fcf2b4p-2f,-0x1.fcf2a6p-2f}, 0x1.889778p-1f}, //     7  
  {{-0x1.fc70f6p-2f,-0x1.fc70e8p-2f}, 0x1.88a776p-1f}, //     7  
  {{-0x1.fbef24p-2f,-0x1.fbef16p-2f}, 0x1.88b774p-1f}, //     7  
  {{-0x1.fb6d30p-2f,-0x1.fb6d22p-2f}, 0x1.88c774p-1f}, //     7  
  {{-0x1.faeb2ap-2f,-0x1.faeb1cp-2f}, 0x1.88d774p-1f}, //     7  
  {{-0x1.fa6920p-2f,-0x1.fa6912p-2f}, 0x1.88e772p-1f}, //     7  
  {{-0x1.f9e706p-2f,-0x1.f9e6f8p-2f}, 0x1.88f770p-1f}, //     7  
  {{-0x1.f964c8p-2f,-0x1.f964bap-2f}, 0x1.890770p-1f}, //     7  
  {{-0x1.f8e278p-2f,-0x1.f8e26ap-2f}, 0x1.891770p-1f}, //     7  
  {{-0x1.f86026p-2f,-0x1.f86018p-2f}, 0x1.89276ep-1f}, //     7  
  {{-0x1.f7ddc2p-2f,-0x1.f7ddb2p-2f}, 0x1.89376cp-1f}, //     8  
  {{-0x1.f75b3ap-2f,-0x1.f75b2cp-2f}, 0x1.89476cp-1f}, //     7  
  {{-0x1.f6d8a0p-2f,-0x1.f6d892p-2f}, 0x1.89576cp-1f}, //     7  
  {{-0x1.f65604p-2f,-0x1.f655f6p-2f}, 0x1.89676ap-1f}, //     7  
  {{-0x1.f5d356p-2f,-0x1.f5d348p-2f}, 0x1.897768p-1f}, //     7  
  {{-0x1.f55084p-2f,-0x1.f55076p-2f}, 0x1.898768p-1f}, //     7  
  {{-0x1.f4cda0p-2f,-0x1.f4cd92p-2f}, 0x1.899768p-1f}, //     7  
  {{-0x1.f44abcp-2f,-0x1.f44aacp-2f}, 0x1.89a766p-1f}, //     8  
  {{-0x1.f3c7c2p-2f,-0x1.f3c7b4p-2f}, 0x1.89b764p-1f}, //     7  
  {{-0x1.f344a8p-2f,-0x1.f3449ap-2f}, 0x1.89c764p-1f}, //     7  
  {{-0x1.f2c17ap-2f,-0x1.f2c16cp-2f}, 0x1.89d764p-1f}, //     7  
  {{-0x1.f23e4cp-2f,-0x1.f23e3cp-2f}, 0x1.89e762p-1f}, //     8  
  {{-0x1.f1bb0ap-2f,-0x1.f1bafap-2f}, 0x1.89f760p-1f}, //     8  
  {{-0x1.f137a4p-2f,-0x1.f13796p-2f}, 0x1.8a0760p-1f}, //     7  
  {{-0x1.f0b42ep-2f,-0x1.f0b41ep-2f}, 0x1.8a1760p-1f}, //     8  
  {{-0x1.f030b4p-2f,-0x1.f030a6p-2f}, 0x1.8a275ep-1f}, //     7  
  {{-0x1.efad28p-2f,-0x1.efad1ap-2f}, 0x1.8a375cp-1f}, //     7  
  {{-0x1.ef297ap-2f,-0x1.ef296cp-2f}, 0x1.8a475cp-1f}, //     7  
  {{-0x1.eea5b8p-2f,-0x1.eea5aap-2f}, 0x1.8a575cp-1f}, //     7  
  {{-0x1.ee21f6p-2f,-0x1.ee21e6p-2f}, 0x1.8a675ap-1f}, //     8  
  {{-0x1.ed9e20p-2f,-0x1.ed9e12p-2f}, 0x1.8a7758p-1f}, //     7  
  {{-0x1.ed1a28p-2f,-0x1.ed1a18p-2f}, 0x1.8a8758p-1f}, //     8  
  {{-0x1.ec961cp-2f,-0x1.ec960ep-2f}, 0x1.8a9758p-1f}, //     7  
  {{-0x1.ec120ep-2f,-0x1.ec1200p-2f}, 0x1.8aa756p-1f}, //     7  
  {{-0x1.eb8df0p-2f,-0x1.eb8de0p-2f}, 0x1.8ab754p-1f}, //     8  
  {{-0x1.eb09acp-2f,-0x1.eb099ep-2f}, 0x1.8ac754p-1f}, //     7  
  {{-0x1.ea8558p-2f,-0x1.ea854ap-2f}, 0x1.8ad754p-1f}, //     7  
  {{-0x1.ea0100p-2f,-0x1.ea00f2p-2f}, 0x1.8ae752p-1f}, //     7  
  {{-0x1.e97c98p-2f,-0x1.e97c88p-2f}, 0x1.8af750p-1f}, //     8  
  {{-0x1.e8f80ap-2f,-0x1.e8f7fcp-2f}, 0x1.8b0750p-1f}, //     7  
  {{-0x1.e8736cp-2f,-0x1.e8735ep-2f}, 0x1.8b1750p-1f}, //     7  
  {{-0x1.e7eecap-2f,-0x1.e7eebcp-2f}, 0x1.8b274ep-1f}, //     7  
  {{-0x1.e76a18p-2f,-0x1.e76a08p-2f}, 0x1.8b374cp-1f}, //     8  
  {{-0x1.e6e540p-2f,-0x1.e6e532p-2f}, 0x1.8b474cp-1f}, //     7  
  {{-0x1.e66058p-2f,-0x1.e66048p-2f}, 0x1.8b574cp-1f}, //     8  
  {{-0x1.e5db6cp-2f,-0x1.e5db5ep-2f}, 0x1.8b674ap-1f}, //     7  
  {{-0x1.e5566ep-2f,-0x1.e55660p-2f}, 0x1.8b7748p-1f}, //     7  
  {{-0x1.e4d14ep-2f,-0x1.e4d140p-2f}, 0x1.8b8748p-1f}, //     7  
  {{-0x1.e44c1ap-2f,-0x1.e44c0cp-2f}, 0x1.8b9748p-1f}, //     7  
  {{-0x1.e3c6e6p-2f,-0x1.e3c6d6p-2f}, 0x1.8ba746p-1f}, //     8  
  {{-0x1.e3419ep-2f,-0x1.e34190p-2f}, 0x1.8bb744p-1f}, //     7  
  {{-0x1.e2bc32p-2f,-0x1.e2bc24p-2f}, 0x1.8bc744p-1f}, //     7  
  {{-0x1.e236b6p-2f,-0x1.e236a6p-2f}, 0x1.8bd744p-1f}, //     8  
  {{-0x1.e1b136p-2f,-0x1.e1b128p-2f}, 0x1.8be742p-1f}, //     7  
  {{-0x1.e12ba4p-2f,-0x1.e12b96p-2f}, 0x1.8bf740p-1f}, //     7  
  {{-0x1.e0a5eep-2f,-0x1.e0a5e0p-2f}, 0x1.8c0740p-1f}, //     7  
  {{-0x1.e02028p-2f,-0x1.e02018p-2f}, 0x1.8c1740p-1f}, //     8  
  {{-0x1.df9a5ep-2f,-0x1.df9a50p-2f}, 0x1.8c273ep-1f}, //     7  
  {{-0x1.df1482p-2f,-0x1.df1472p-2f}, 0x1.8c373cp-1f}, //     8  
  {{-0x1.de8e82p-2f,-0x1.de8e74p-2f}, 0x1.8c473cp-1f}, //     7  
  {{-0x1.de0870p-2f,-0x1.de0862p-2f}, 0x1.8c573cp-1f}, //     7  
  {{-0x1.dd825cp-2f,-0x1.dd824ep-2f}, 0x1.8c673ap-1f}, //     7  
  {{-0x1.dcfc36p-2f,-0x1.dcfc28p-2f}, 0x1.8c7738p-1f}, //     7  
  {{-0x1.dc75ecp-2f,-0x1.dc75dep-2f}, 0x1.8c8738p-1f}, //     7  
  {{-0x1.dbef90p-2f,-0x1.dbef82p-2f}, 0x1.8c9738p-1f}, //     7  
  {{-0x1.db6932p-2f,-0x1.db6924p-2f}, 0x1.8ca736p-1f}, //     7  
  {{-0x1.dae2c2p-2f,-0x1.dae2b2p-2f}, 0x1.8cb734p-1f}, //     8  
  {{-0x1.da5c2ep-2f,-0x1.da5c1ep-2f}, 0x1.8cc734p-1f}, //     8  
  {{-0x1.d9d586p-2f,-0x1.d9d578p-2f}, 0x1.8cd734p-1f}, //     7  
  {{-0x1.d94edep-2f,-0x1.d94ed0p-2f}, 0x1.8ce732p-1f}, //     7  
  {{-0x1.d8c824p-2f,-0x1.d8c814p-2f}, 0x1.8cf730p-1f}, //     8  
  {{-0x1.d84146p-2f,-0x1.d84136p-2f}, 0x1.8d0730p-1f}, //     8  
  {{-0x1.d7ba54p-2f,-0x1.d7ba46p-2f}, 0x1.8d1730p-1f}, //     7  
  {{-0x1.d73362p-2f,-0x1.d73352p-2f}, 0x1.8d272ep-1f}, //     8  
  {{-0x1.d6ac5cp-2f,-0x1.d6ac4ep-2f}, 0x1.8d372cp-1f}, //     7  
  {{-0x1.d62534p-2f,-0x1.d62524p-2f}, 0x1.8d472cp-1f}, //     8  
  {{-0x1.d59df8p-2f,-0x1.d59de8p-2f}, 0x1.8d572cp-1f}, //     8  
  {{-0x1.d516bap-2f,-0x1.d516acp-2f}, 0x1.8d672ap-1f}, //     7  
  {{-0x1.d48f6ap-2f,-0x1.d48f5cp-2f}, 0x1.8d7728p-1f}, //     7  
  {{-0x1.d407f8p-2f,-0x1.d407e8p-2f}, 0x1.8d8728p-1f}, //     8  
  {{-0x1.d38072p-2f,-0x1.d38062p-2f}, 0x1.8d9728p-1f}, //     8  
  {{-0x1.d2f8eap-2f,-0x1.d2f8dap-2f}, 0x1.8da726p-1f}, //     8  
  {{-0x1.d27150p-2f,-0x1.d27140p-2f}, 0x1.8db724p-1f}, //     8  
  {{-0x1.d1e992p-2f,-0x1.d1e982p-2f}, 0x1.8dc724p-1f}, //     8  
  {{-0x1.d161c0p-2f,-0x1.d161b2p-2f}, 0x1.8dd724p-1f}, //     7  
  {{-0x1.d0d9eep-2f,-0x1.d0d9e0p-2f}, 0x1.8de722p-1f}, //     7  
  {{-0x1.d0520ap-2f,-0x1.d051fap-2f}, 0x1.8df720p-1f}, //     8  
  {{-0x1.cfca02p-2f,-0x1.cfc9f2p-2f}, 0x1.8e0720p-1f}, //     8  
  {{-0x1.cf41e6p-2f,-0x1.cf41d8p-2f}, 0x1.8e1720p-1f}, //     7  
  {{-0x1.ceb9cap-2f,-0x1.ceb9bap-2f}, 0x1.8e271ep-1f}, //     8  
  {{-0x1.ce319ap-2f,-0x1.ce318cp-2f}, 0x1.8e371cp-1f}, //     7  
  {{-0x1.cda948p-2f,-0x1.cda938p-2f}, 0x1.8e471cp-1f}, //     8  
  {{-0x1.cd20e2p-2f,-0x1.cd20d2p-2f}, 0x1.8e571cp-1f}, //     8  
  {{-0x1.cc987ap-2f,-0x1.cc986ap-2f}, 0x1.8e671ap-1f}, //     8  
  {{-0x1.cc1000p-2f,-0x1.cc0ff0p-2f}, 0x1.8e7718p-1f}, //     8  
  {{-0x1.cb8762p-2f,-0x1.cb8752p-2f}, 0x1.8e8718p-1f}, //     8  
  {{-0x1.cafeb2p-2f,-0x1.cafea2p-2f}, 0x1.8e9718p-1f}, //     8  
  {{-0x1.ca7600p-2f,-0x1.ca75f0p-2f}, 0x1.8ea716p-1f}, //     8  
  {{-0x1.c9ed3ap-2f,-0x1.c9ed2cp-2f}, 0x1.8eb714p-1f}, //     7  
  {{-0x1.c96452p-2f,-0x1.c96444p-2f}, 0x1.8ec714p-1f}, //     7  
  {{-0x1.c8db58p-2f,-0x1.c8db48p-2f}, 0x1.8ed714p-1f}, //     8  
  {{-0x1.c8525ap-2f,-0x1.c8524cp-2f}, 0x1.8ee712p-1f}, //     7  
  {{-0x1.c7c94ap-2f,-0x1.c7c93cp-2f}, 0x1.8ef710p-1f}, //     7  
  {{-0x1.c74018p-2f,-0x1.c74008p-2f}, 0x1.8f0710p-1f}, //     8  
  {{-0x1.c6b6d2p-2f,-0x1.c6b6c2p-2f}, 0x1.8f1710p-1f}, //     8  
  {{-0x1.c62d8ap-2f,-0x1.c62d7ap-2f}, 0x1.8f270ep-1f}, //     8  
  {{-0x1.c5a430p-2f,-0x1.c5a420p-2f}, 0x1.8f370cp-1f}, //     8  
  {{-0x1.c51ab2p-2f,-0x1.c51aa2p-2f}, 0x1.8f470cp-1f}, //     8  
  {{-0x1.c49122p-2f,-0x1.c49112p-2f}, 0x1.8f570cp-1f}, //     8  
  {{-0x1.c4078ep-2f,-0x1.c40780p-2f}, 0x1.8f670ap-1f}, //     7  
  {{-0x1.c37deap-2f,-0x1.c37ddap-2f}, 0x1.8f7708p-1f}, //     8  
  {{-0x1.c2f420p-2f,-0x1.c2f412p-2f}, 0x1.8f8708p-1f}, //     7  
  {{-0x1.c26a46p-2f,-0x1.c26a36p-2f}, 0x1.8f9708p-1f}, //     8  
  {{-0x1.c1e068p-2f,-0x1.c1e058p-2f}, 0x1.8fa706p-1f}, //     8  
  {{-0x1.c15678p-2f,-0x1.c15668p-2f}, 0x1.8fb704p-1f}, //     8  
  {{-0x1.c0cc64p-2f,-0x1.c0cc54p-2f}, 0x1.8fc704p-1f}, //     8  
  {{-0x1.c0423ep-2f,-0x1.c0422ep-2f}, 0x1.8fd704p-1f}, //     8  
  {{-0x1.bfb816p-2f,-0x1.bfb806p-2f}, 0x1.8fe702p-1f}, //     8  
  {{-0x1.bf2ddap-2f,-0x1.bf2dccp-2f}, 0x1.8ff700p-1f}, //     7  
  {{-0x1.bea37cp-2f,-0x1.bea36cp-2f}, 0x1.900700p-1f}, //     8  
  {{-0x1.be190ap-2f,-0x1.be18fap-2f}, 0x1.901700p-1f}, //     8  
  {{-0x1.bd8e98p-2f,-0x1.bd8e88p-2f}, 0x1.9026fep-1f}, //     8  
  {{-0x1.bd0412p-2f,-0x1.bd0402p-2f}, 0x1.9036fcp-1f}, //     8  
  {{-0x1.bc7968p-2f,-0x1.bc7958p-2f}, 0x1.9046fcp-1f}, //     8  
  {{-0x1.bbeeacp-2f,-0x1.bbee9cp-2f}, 0x1.9056fcp-1f}, //     8  
  {{-0x1.bb63eep-2f,-0x1.bb63dep-2f}, 0x1.9066fap-1f}, //     8  
  {{-0x1.bad91cp-2f,-0x1.bad90cp-2f}, 0x1.9076f8p-1f}, //     8  
  {{-0x1.ba4e28p-2f,-0x1.ba4e18p-2f}, 0x1.9086f8p-1f}, //     8  
  {{-0x1.b9c320p-2f,-0x1.b9c310p-2f}, 0x1.9096f8p-1f}, //     8  
  {{-0x1.b93816p-2f,-0x1.b93808p-2f}, 0x1.90a6f6p-1f}, //     7  
  {{-0x1.b8acfap-2f,-0x1.b8acecp-2f}, 0x1.90b6f4p-1f}, //     7  
  {{-0x1.b821bap-2f,-0x1.b821acp-2f}, 0x1.90c6f4p-1f}, //     7  
  {{-0x1.b79668p-2f,-0x1.b79658p-2f}, 0x1.90d6f4p-1f}, //     8  
  {{-0x1.b70b14p-2f,-0x1.b70b04p-2f}, 0x1.90e6f2p-1f}, //     8  
  {{-0x1.b67faep-2f,-0x1.b67f9ep-2f}, 0x1.90f6f0p-1f}, //     8  
  {{-0x1.b5f422p-2f,-0x1.b5f412p-2f}, 0x1.9106f0p-1f}, //     8  
  {{-0x1.b56884p-2f,-0x1.b56874p-2f}, 0x1.9116f0p-1f}, //     8  
  {{-0x1.b4dce4p-2f,-0x1.b4dcd6p-2f}, 0x1.9126eep-1f}, //     7  
  {{-0x1.b45132p-2f,-0x1.b45124p-2f}, 0x1.9136ecp-1f}, //     7  
  {{-0x1.b3c55cp-2f,-0x1.b3c54ep-2f}, 0x1.9146ecp-1f}, //     7  
  {{-0x1.b33974p-2f,-0x1.b33964p-2f}, 0x1.9156ecp-1f}, //     8  
  {{-0x1.b2ad8ap-2f,-0x1.b2ad7ap-2f}, 0x1.9166eap-1f}, //     8  
  {{-0x1.b2218cp-2f,-0x1.b2217cp-2f}, 0x1.9176e8p-1f}, //     8  
  {{-0x1.b1956ap-2f,-0x1.b1955ap-2f}, 0x1.9186e8p-1f}, //     8  
  {{-0x1.b10936p-2f,-0x1.b10926p-2f}, 0x1.9196e8p-1f}, //     8  
  {{-0x1.b07d00p-2f,-0x1.b07cf0p-2f}, 0x1.91a6e6p-1f}, //     8  
  {{-0x1.aff0b8p-2f,-0x1.aff0a8p-2f}, 0x1.91b6e4p-1f}, //     8  
  {{-0x1.af644ap-2f,-0x1.af643cp-2f}, 0x1.91c6e4p-1f}, //     7  
  {{-0x1.aed7ccp-2f,-0x1.aed7bcp-2f}, 0x1.91d6e4p-1f}, //     8  
  {{-0x1.ae4b4ap-2f,-0x1.ae4b3ap-2f}, 0x1.91e6e2p-1f}, //     8  
  {{-0x1.adbeb6p-2f,-0x1.adbea6p-2f}, 0x1.91f6e0p-1f}, //     8  
  {{-0x1.ad31fep-2f,-0x1.ad31eep-2f}, 0x1.9206e0p-1f}, //     8  
  {{-0x1.aca534p-2f,-0x1.aca524p-2f}, 0x1.9216e0p-1f}, //     8  
  {{-0x1.ac1868p-2f,-0x1.ac1858p-2f}, 0x1.9226dep-1f}, //     8  
  {{-0x1.ab8b88p-2f,-0x1.ab8b78p-2f}, 0x1.9236dcp-1f}, //     8  
  {{-0x1.aafe84p-2f,-0x1.aafe74p-2f}, 0x1.9246dcp-1f}, //     8  
  {{-0x1.aa716ep-2f,-0x1.aa715ep-2f}, 0x1.9256dcp-1f}, //     8  
  {{-0x1.a9e456p-2f,-0x1.a9e446p-2f}, 0x1.9266dap-1f}, //     8  
  {{-0x1.a9572cp-2f,-0x1.a9571cp-2f}, 0x1.9276d8p-1f}, //     8  
  {{-0x1.a8c9dcp-2f,-0x1.a8c9cep-2f}, 0x1.9286d8p-1f}, //     7  
  {{-0x1.a83c7cp-2f,-0x1.a83c6cp-2f}, 0x1.9296d8p-1f}, //     8  
  {{-0x1.a7af18p-2f,-0x1.a7af08p-2f}, 0x1.92a6d6p-1f}, //     8  
  {{-0x1.a721a2p-2f,-0x1.a72192p-2f}, 0x1.92b6d4p-1f}, //     8  
  {{-0x1.a69408p-2f,-0x1.a693f8p-2f}, 0x1.92c6d4p-1f}, //     8  
  {{-0x1.a6065ap-2f,-0x1.a6064ap-2f}, 0x1.92d6d4p-1f}, //     8  
  {{-0x1.a578acp-2f,-0x1.a5789cp-2f}, 0x1.92e6d2p-1f}, //     8  
  {{-0x1.a4eaeap-2f,-0x1.a4eadap-2f}, 0x1.92f6d0p-1f}, //     8  
  {{-0x1.a45d04p-2f,-0x1.a45cf4p-2f}, 0x1.9306d0p-1f}, //     8  
  {{-0x1.a3cf0cp-2f,-0x1.a3cefcp-2f}, 0x1.9316d0p-1f}, //     8  
  {{-0x1.a34112p-2f,-0x1.a34102p-2f}, 0x1.9326cep-1f}, //     8  
  {{-0x1.a2b304p-2f,-0x1.a2b2f6p-2f}, 0x1.9336ccp-1f}, //     7  
  {{-0x1.a224d4p-2f,-0x1.a224c4p-2f}, 0x1.9346ccp-1f}, //     8  
  {{-0x1.a1968ep-2f,-0x1.a19680p-2f}, 0x1.9356ccp-1f}, //     7  
  {{-0x1.a1084ap-2f,-0x1.a1083ap-2f}, 0x1.9366cap-1f}, //     8  
  {{-0x1.a079f0p-2f,-0x1.a079e2p-2f}, 0x1.9376c8p-1f}, //     7  
  {{-0x1.9feb74p-2f,-0x1.9feb64p-2f}, 0x1.9386c8p-1f}, //     8  
  {{-0x1.9f5ce4p-2f,-0x1.9f5cd4p-2f}, 0x1.9396c8p-1f}, //     8  
  {{-0x1.9ece52p-2f,-0x1.9ece42p-2f}, 0x1.93a6c6p-1f}, //     8  
  {{-0x1.9e3faep-2f,-0x1.9e3f9ep-2f}, 0x1.93b6c4p-1f}, //     8  
  {{-0x1.9db0e6p-2f,-0x1.9db0d6p-2f}, 0x1.93c6c4p-1f}, //     8  
  {{-0x1.9d220ap-2f,-0x1.9d21fap-2f}, 0x1.93d6c4p-1f}, //     8  
  {{-0x1.9c932ep-2f,-0x1.9c931ep-2f}, 0x1.93e6c2p-1f}, //     8  
  {{-0x1.9c043ep-2f,-0x1.9c042ep-2f}, 0x1.93f6c0p-1f}, //     8  
  {{-0x1.9b7528p-2f,-0x1.9b751ap-2f}, 0x1.9406c0p-1f}, //     7  
  {{-0x1.9ae602p-2f,-0x1.9ae5f2p-2f}, 0x1.9416c0p-1f}, //     8  
  {{-0x1.9a56dap-2f,-0x1.9a56cap-2f}, 0x1.9426bep-1f}, //     8  
  {{-0x1.99c79ep-2f,-0x1.99c78ep-2f}, 0x1.9436bcp-1f}, //     8  
  {{-0x1.99383ep-2f,-0x1.99382ep-2f}, 0x1.9446bcp-1f}, //     8  
  {{-0x1.98a8cap-2f,-0x1.98a8bap-2f}, 0x1.9456bcp-1f}, //     8  
  {{-0x1.981956p-2f,-0x1.981946p-2f}, 0x1.9466bap-1f}, //     8  
  {{-0x1.9789cep-2f,-0x1.9789bep-2f}, 0x1.9476b8p-1f}, //     8  
  {{-0x1.96fa22p-2f,-0x1.96fa12p-2f}, 0x1.9486b8p-1f}, //     8  
  {{-0x1.966a64p-2f,-0x1.966a54p-2f}, 0x1.9496b8p-1f}, //     8  
  {{-0x1.95daa4p-2f,-0x1.95da94p-2f}, 0x1.94a6b6p-1f}, //     8  
  {{-0x1.954ad0p-2f,-0x1.954ac0p-2f}, 0x1.94b6b4p-1f}, //     8  
  {{-0x1.94bad8p-2f,-0x1.94bac8p-2f}, 0x1.94c6b4p-1f}, //     8  
  {{-0x1.942acep-2f,-0x1.942abep-2f}, 0x1.94d6b4p-1f}, //     8  
  {{-0x1.939ac2p-2f,-0x1.939ab2p-2f}, 0x1.94e6b2p-1f}, //     8  
  {{-0x1.930aa4p-2f,-0x1.930a94p-2f}, 0x1.94f6b0p-1f}, //     8  
  {{-0x1.927a60p-2f,-0x1.927a50p-2f}, 0x1.9506b0p-1f}, //     8  
  {{-0x1.91ea08p-2f,-0x1.91e9f8p-2f}, 0x1.9516b0p-1f}, //     8  
  {{-0x1.9159b0p-2f,-0x1.9159a0p-2f}, 0x1.9526aep-1f}, //     8  
  {{-0x1.90c946p-2f,-0x1.90c936p-2f}, 0x1.9536acp-1f}, //     8  
  {{-0x1.9038b6p-2f,-0x1.9038a6p-2f}, 0x1.9546acp-1f}, //     8  
  {{-0x1.8fa814p-2f,-0x1.8fa804p-2f}, 0x1.9556acp-1f}, //     8  
  {{-0x1.8f1770p-2f,-0x1.8f1760p-2f}, 0x1.9566aap-1f}, //     8  
  {{-0x1.8e86bap-2f,-0x1.8e86aap-2f}, 0x1.9576a8p-1f}, //     8  
  {{-0x1.8df5dep-2f,-0x1.8df5cep-2f}, 0x1.9586a8p-1f}, //     8  
  {{-0x1.8d64eep-2f,-0x1.8d64dep-2f}, 0x1.9596a8p-1f}, //     8  
  {{-0x1.8cd400p-2f,-0x1.8cd3f0p-2f}, 0x1.95a6a6p-1f}, //     8  
  {{-0x1.8c42fcp-2f,-0x1.8c42ecp-2f}, 0x1.95b6a4p-1f}, //     8  
  {{-0x1.8bb1d4p-2f,-0x1.8bb1c4p-2f}, 0x1.95c6a4p-1f}, //     8  
  {{-0x1.8b209ap-2f,-0x1.8b208ap-2f}, 0x1.95d6a4p-1f}, //     8  
  {{-0x1.8a8f5ep-2f,-0x1.8a8f4ep-2f}, 0x1.95e6a2p-1f}, //     8  
  {{-0x1.89fe10p-2f,-0x1.89fe00p-2f}, 0x1.95f6a0p-1f}, //     8  
  {{-0x1.896c9cp-2f,-0x1.896c8cp-2f}, 0x1.9606a0p-1f}, //     8  
  {{-0x1.88db14p-2f,-0x1.88db04p-2f}, 0x1.9616a0p-1f}, //     8  
  {{-0x1.88498ep-2f,-0x1.88497cp-2f}, 0x1.96269ep-1f}, //     9  
  {{-0x1.87b7f2p-2f,-0x1.87b7e2p-2f}, 0x1.96369cp-1f}, //     8  
  {{-0x1.872632p-2f,-0x1.872622p-2f}, 0x1.96469cp-1f}, //     8  
  {{-0x1.869460p-2f,-0x1.869450p-2f}, 0x1.96569cp-1f}, //     8  
  {{-0x1.86028cp-2f,-0x1.86027cp-2f}, 0x1.96669ap-1f}, //     8  
  {{-0x1.8570a4p-2f,-0x1.857094p-2f}, 0x1.967698p-1f}, //     8  
  {{-0x1.84de98p-2f,-0x1.84de88p-2f}, 0x1.968698p-1f}, //     8  
  {{-0x1.844c7ap-2f,-0x1.844c6ap-2f}, 0x1.969698p-1f}, //     8  
  {{-0x1.83ba5ap-2f,-0x1.83ba4ap-2f}, 0x1.96a696p-1f}, //     8  
  {{-0x1.832826p-2f,-0x1.832816p-2f}, 0x1.96b694p-1f}, //     8  
  {{-0x1.8295cep-2f,-0x1.8295bep-2f}, 0x1.96c694p-1f}, //     8  
  {{-0x1.820362p-2f,-0x1.820352p-2f}, 0x1.96d694p-1f}, //     8  
  {{-0x1.8170f6p-2f,-0x1.8170e6p-2f}, 0x1.96e692p-1f}, //     8  
  {{-0x1.80de76p-2f,-0x1.80de66p-2f}, 0x1.96f690p-1f}, //     8  
  {{-0x1.804bd2p-2f,-0x1.804bc2p-2f}, 0x1.970690p-1f}, //     8  
  {{-0x1.7fb91ap-2f,-0x1.7fb90ap-2f}, 0x1.971690p-1f}, //     8  
  {{-0x1.7f2662p-2f,-0x1.7f2652p-2f}, 0x1.97268ep-1f}, //     8  
  {{-0x1.7e9396p-2f,-0x1.7e9386p-2f}, 0x1.97368cp-1f}, //     8  
  {{-0x1.7e00a6p-2f,-0x1.7e0096p-2f}, 0x1.97468cp-1f}, //     8  
  {{-0x1.7d6da2p-2f,-0x1.7d6d92p-2f}, 0x1.97568cp-1f}, //     8  
  {{-0x1.7cda9cp-2f,-0x1.7cda8cp-2f}, 0x1.97668ap-1f}, //     8  
  {{-0x1.7c4784p-2f,-0x1.7c4774p-2f}, 0x1.977688p-1f}, //     8  
  {{-0x1.7bb448p-2f,-0x1.7bb438p-2f}, 0x1.978688p-1f}, //     8  
  {{-0x1.7b20f8p-2f,-0x1.7b20e6p-2f}, 0x1.979688p-1f}, //     9  
  {{-0x1.7a8da6p-2f,-0x1.7a8d96p-2f}, 0x1.97a686p-1f}, //     8  
  {{-0x1.79fa42p-2f,-0x1.79fa32p-2f}, 0x1.97b684p-1f}, //     8  
  {{-0x1.7966b8p-2f,-0x1.7966a8p-2f}, 0x1.97c684p-1f}, //     8  
  {{-0x1.78d31cp-2f,-0x1.78d30ap-2f}, 0x1.97d684p-1f}, //     9  
  {{-0x1.783f7ep-2f,-0x1.783f6ep-2f}, 0x1.97e682p-1f}, //     8  
  {{-0x1.77abcep-2f,-0x1.77abbcp-2f}, 0x1.97f680p-1f}, //     9  
  {{-0x1.7717f6p-2f,-0x1.7717e6p-2f}, 0x1.980680p-1f}, //     8  
  {{-0x1.76840ep-2f,-0x1.7683fep-2f}, 0x1.981680p-1f}, //     8  
  {{-0x1.75f024p-2f,-0x1.75f014p-2f}, 0x1.98267ep-1f}, //     8  
  {{-0x1.755c26p-2f,-0x1.755c16p-2f}, 0x1.98367cp-1f}, //     8  
  {{-0x1.74c804p-2f,-0x1.74c7f4p-2f}, 0x1.98467cp-1f}, //     8  
  {{-0x1.7433cep-2f,-0x1.7433bep-2f}, 0x1.98567cp-1f}, //     8  
  {{-0x1.739f98p-2f,-0x1.739f88p-2f}, 0x1.98667ap-1f}, //     8  
  {{-0x1.730b4ep-2f,-0x1.730b3ep-2f}, 0x1.987678p-1f}, //     8  
  {{-0x1.7276dep-2f,-0x1.7276cep-2f}, 0x1.988678p-1f}, //     8  
  {{-0x1.71e25cp-2f,-0x1.71e24cp-2f}, 0x1.989678p-1f}, //     8  
  {{-0x1.714ddap-2f,-0x1.714dc8p-2f}, 0x1.98a676p-1f}, //     9  
  {{-0x1.70b944p-2f,-0x1.70b932p-2f}, 0x1.98b674p-1f}, //     9  
  {{-0x1.702488p-2f,-0x1.702476p-2f}, 0x1.98c674p-1f}, //     9  
  {{-0x1.6f8fb8p-2f,-0x1.6f8fa8p-2f}, 0x1.98d674p-1f}, //     8  
  {{-0x1.6efae8p-2f,-0x1.6efad8p-2f}, 0x1.98e672p-1f}, //     8  
  {{-0x1.6e6606p-2f,-0x1.6e65f6p-2f}, 0x1.98f670p-1f}, //     8  
  {{-0x1.6dd0fep-2f,-0x1.6dd0ecp-2f}, 0x1.990670p-1f}, //     9  
  {{-0x1.6d3be2p-2f,-0x1.6d3bd2p-2f}, 0x1.991670p-1f}, //     8  
  {{-0x1.6ca6c6p-2f,-0x1.6ca6b6p-2f}, 0x1.99266ep-1f}, //     8  
  {{-0x1.6c1196p-2f,-0x1.6c1186p-2f}, 0x1.99366cp-1f}, //     8  
  {{-0x1.6b7c42p-2f,-0x1.6b7c30p-2f}, 0x1.99466cp-1f}, //     9  
  {{-0x1.6ae6d8p-2f,-0x1.6ae6c8p-2f}, 0x1.99566cp-1f}, //     8  
  {{-0x1.6a5170p-2f,-0x1.6a5160p-2f}, 0x1.99666ap-1f}, //     8  
  {{-0x1.69bbf4p-2f,-0x1.69bbe4p-2f}, 0x1.997668p-1f}, //     8  
  {{-0x1.692652p-2f,-0x1.692642p-2f}, 0x1.998668p-1f}, //     8  
  {{-0x1.68909cp-2f,-0x1.68908cp-2f}, 0x1.999668p-1f}, //     8  
  {{-0x1.67fae8p-2f,-0x1.67fad6p-2f}, 0x1.99a666p-1f}, //     9  
  {{-0x1.67651ep-2f,-0x1.67650ep-2f}, 0x1.99b664p-1f}, //     8  
  {{-0x1.66cf30p-2f,-0x1.66cf20p-2f}, 0x1.99c664p-1f}, //     8  
  {{-0x1.66392ep-2f,-0x1.66391ep-2f}, 0x1.99d664p-1f}, //     8  
  {{-0x1.65a32cp-2f,-0x1.65a31ap-2f}, 0x1.99e662p-1f}, //     9  
  {{-0x1.650d16p-2f,-0x1.650d06p-2f}, 0x1.99f660p-1f}, //     8  
  {{-0x1.6476dap-2f,-0x1.6476cap-2f}, 0x1.9a0660p-1f}, //     8  
  {{-0x1.63e08cp-2f,-0x1.63e07ap-2f}, 0x1.9a1660p-1f}, //     9  
  {{-0x1.634a3cp-2f,-0x1.634a2cp-2f}, 0x1.9a265ep-1f}, //     8  
  {{-0x1.62b3dap-2f,-0x1.62b3cap-2f}, 0x1.9a365cp-1f}, //     8  
  {{-0x1.621d52p-2f,-0x1.621d40p-2f}, 0x1.9a465cp-1f}, //     9  
  {{-0x1.6186b6p-2f,-0x1.6186a6p-2f}, 0x1.9a565cp-1f}, //     8  
  {{-0x1.60f01ap-2f,-0x1.60f00ap-2f}, 0x1.9a665ap-1f}, //     8  
  {{-0x1.60596ap-2f,-0x1.60595ap-2f}, 0x1.9a7658p-1f}, //     8  
  {{-0x1.5fc296p-2f,-0x1.5fc284p-2f}, 0x1.9a8658p-1f}, //     9  
  {{-0x1.5f2bacp-2f,-0x1.5f2b9cp-2f}, 0x1.9a9658p-1f}, //     8  
  {{-0x1.5e94c4p-2f,-0x1.5e94b2p-2f}, 0x1.9aa656p-1f}, //     9  
  {{-0x1.5dfdc8p-2f,-0x1.5dfdb6p-2f}, 0x1.9ab654p-1f}, //     9  
  {{-0x1.5d66a6p-2f,-0x1.5d6694p-2f}, 0x1.9ac654p-1f}, //     9  
  {{-0x1.5ccf70p-2f,-0x1.5ccf5ep-2f}, 0x1.9ad654p-1f}, //     9  
  {{-0x1.5c383ap-2f,-0x1.5c3828p-2f}, 0x1.9ae652p-1f}, //     9  
  {{-0x1.5ba0f0p-2f,-0x1.5ba0e0p-2f}, 0x1.9af650p-1f}, //     8  
  {{-0x1.5b0982p-2f,-0x1.5b0970p-2f}, 0x1.9b0650p-1f}, //     9  
  {{-0x1.5a71fep-2f,-0x1.5a71eep-2f}, 0x1.9b1650p-1f}, //     8  
  {{-0x1.59da7cp-2f,-0x1.59da6ap-2f}, 0x1.9b264ep-1f}, //     9  
  {{-0x1.5942e6p-2f,-0x1.5942d4p-2f}, 0x1.9b364cp-1f}, //     9  
  {{-0x1.58ab2ap-2f,-0x1.58ab18p-2f}, 0x1.9b464cp-1f}, //     9  
  {{-0x1.58135ap-2f,-0x1.581348p-2f}, 0x1.9b564cp-1f}, //     9  
  {{-0x1.577b8ap-2f,-0x1.577b78p-2f}, 0x1.9b664ap-1f}, //     9  
  {{-0x1.56e3a6p-2f,-0x1.56e396p-2f}, 0x1.9b7648p-1f}, //     8  
  {{-0x1.564b9cp-2f,-0x1.564b8cp-2f}, 0x1.9b8648p-1f}, //     8  
  {{-0x1.55b380p-2f,-0x1.55b36ep-2f}, 0x1.9b9648p-1f}, //     9  
  {{-0x1.551b62p-2f,-0x1.551b52p-2f}, 0x1.9ba646p-1f}, //     8  
  {{-0x1.548332p-2f,-0x1.548322p-2f}, 0x1.9bb644p-1f}, //     8  
  {{-0x1.53eadcp-2f,-0x1.53eacap-2f}, 0x1.9bc644p-1f}, //     9  
  {{-0x1.535272p-2f,-0x1.535260p-2f}, 0x1.9bd644p-1f}, //     9  
  {{-0x1.52ba08p-2f,-0x1.52b9f6p-2f}, 0x1.9be642p-1f}, //     9  
  {{-0x1.52218ap-2f,-0x1.52217ap-2f}, 0x1.9bf640p-1f}, //     8  
  {{-0x1.5188e6p-2f,-0x1.5188d6p-2f}, 0x1.9c0640p-1f}, //     8  
  {{-0x1.50f030p-2f,-0x1.50f01ep-2f}, 0x1.9c1640p-1f}, //     9  
  {{-0x1.505778p-2f,-0x1.505766p-2f}, 0x1.9c263ep-1f}, //     9  
  {{-0x1.4fbeacp-2f,-0x1.4fbe9cp-2f}, 0x1.9c363cp-1f}, //     8  
  {{-0x1.4f25bcp-2f,-0x1.4f25aap-2f}, 0x1.9c463cp-1f}, //     9  
  {{-0x1.4e8cb8p-2f,-0x1.4e8ca6p-2f}, 0x1.9c563cp-1f}, //     9  
  {{-0x1.4df3b2p-2f,-0x1.4df3a2p-2f}, 0x1.9c663ap-1f}, //     8  
  {{-0x1.4d5a9ap-2f,-0x1.4d5a8ap-2f}, 0x1.9c7638p-1f}, //     8  
  {{-0x1.4cc15cp-2f,-0x1.4cc14cp-2f}, 0x1.9c8638p-1f}, //     8  
  {{-0x1.4c280ap-2f,-0x1.4c27fap-2f}, 0x1.9c9638p-1f}, //     8  
  {{-0x1.4b8eb8p-2f,-0x1.4b8ea8p-2f}, 0x1.9ca636p-1f}, //     8  
  {{-0x1.4af554p-2f,-0x1.4af542p-2f}, 0x1.9cb634p-1f}, //     9  
  {{-0x1.4a5bc8p-2f,-0x1.4a5bb6p-2f}, 0x1.9cc634p-1f}, //     9  
  {{-0x1.49c228p-2f,-0x1.49c218p-2f}, 0x1.9cd634p-1f}, //     8  
  {{-0x1.49288ap-2f,-0x1.492878p-2f}, 0x1.9ce632p-1f}, //     9  
  {{-0x1.488ed6p-2f,-0x1.488ec6p-2f}, 0x1.9cf630p-1f}, //     8  
  {{-0x1.47f4fep-2f,-0x1.47f4ecp-2f}, 0x1.9d0630p-1f}, //     9  
  {{-0x1.475b10p-2f,-0x1.475b00p-2f}, 0x1.9d1630p-1f}, //     8  
  {{-0x1.46c124p-2f,-0x1.46c112p-2f}, 0x1.9d262ep-1f}, //     9  
  {{-0x1.462724p-2f,-0x1.462712p-2f}, 0x1.9d362cp-1f}, //     9  
  {{-0x1.458cfep-2f,-0x1.458cecp-2f}, 0x1.9d462cp-1f}, //     9  
  {{-0x1.44f2c4p-2f,-0x1.44f2b2p-2f}, 0x1.9d562cp-1f}, //     9  
  {{-0x1.44588ap-2f,-0x1.445878p-2f}, 0x1.9d662ap-1f}, //     9  
  {{-0x1.43be3cp-2f,-0x1.43be2ap-2f}, 0x1.9d7628p-1f}, //     9  
  {{-0x1.4323c8p-2f,-0x1.4323b6p-2f}, 0x1.9d8628p-1f}, //     9  
  {{-0x1.428940p-2f,-0x1.428930p-2f}, 0x1.9d9628p-1f}, //     8  
  {{-0x1.41eeb8p-2f,-0x1.41eea8p-2f}, 0x1.9da626p-1f}, //     8  
  {{-0x1.41541ep-2f,-0x1.41540cp-2f}, 0x1.9db624p-1f}, //     9  
  {{-0x1.40b95cp-2f,-0x1.40b94ap-2f}, 0x1.9dc624p-1f}, //     9  
  {{-0x1.401e88p-2f,-0x1.401e76p-2f}, 0x1.9dd624p-1f}, //     9  
  {{-0x1.3f83b2p-2f,-0x1.3f83a0p-2f}, 0x1.9de622p-1f}, //     9  
  {{-0x1.3ee8cap-2f,-0x1.3ee8b8p-2f}, 0x1.9df620p-1f}, //     9  
  {{-0x1.3e4dbap-2f,-0x1.3e4daap-2f}, 0x1.9e0620p-1f}, //     8  
  {{-0x1.3db298p-2f,-0x1.3db286p-2f}, 0x1.9e1620p-1f}, //     9  
  {{-0x1.3d1776p-2f,-0x1.3d1764p-2f}, 0x1.9e261ep-1f}, //     9  
  {{-0x1.3c7c40p-2f,-0x1.3c7c2ep-2f}, 0x1.9e361cp-1f}, //     9  
  {{-0x1.3be0e2p-2f,-0x1.3be0d2p-2f}, 0x1.9e461cp-1f}, //     8  
  {{-0x1.3b4572p-2f,-0x1.3b4560p-2f}, 0x1.9e561cp-1f}, //     9  
  {{-0x1.3aaa02p-2f,-0x1.3aa9f0p-2f}, 0x1.9e661ap-1f}, //     9  
  {{-0x1.3a0e7ep-2f,-0x1.3a0e6cp-2f}, 0x1.9e7618p-1f}, //     9  
  {{-0x1.3972d4p-2f,-0x1.3972c2p-2f}, 0x1.9e8618p-1f}, //     9  
  {{-0x1.38d716p-2f,-0x1.38d704p-2f}, 0x1.9e9618p-1f}, //     9  
  {{-0x1.383b58p-2f,-0x1.383b46p-2f}, 0x1.9ea616p-1f}, //     9  
  {{-0x1.379f86p-2f,-0x1.379f76p-2f}, 0x1.9eb614p-1f}, //     8  
  {{-0x1.37038ep-2f,-0x1.37037cp-2f}, 0x1.9ec614p-1f}, //     9  
  {{-0x1.366782p-2f,-0x1.366772p-2f}, 0x1.9ed614p-1f}, //     8  
  {{-0x1.35cb78p-2f,-0x1.35cb66p-2f}, 0x1.9ee612p-1f}, //     9  
  {{-0x1.352f58p-2f,-0x1.352f46p-2f}, 0x1.9ef610p-1f}, //     9  
  {{-0x1.349312p-2f,-0x1.349300p-2f}, 0x1.9f0610p-1f}, //     9  
  {{-0x1.33f6b8p-2f,-0x1.33f6a6p-2f}, 0x1.9f1610p-1f}, //     9  
  {{-0x1.335a60p-2f,-0x1.335a4ep-2f}, 0x1.9f260ep-1f}, //     9  
  {{-0x1.32bdf2p-2f,-0x1.32bde0p-2f}, 0x1.9f360cp-1f}, //     9  
  {{-0x1.32215ep-2f,-0x1.32214cp-2f}, 0x1.9f460cp-1f}, //     9  
  {{-0x1.3184b8p-2f,-0x1.3184a6p-2f}, 0x1.9f560cp-1f}, //     9  
  {{-0x1.30e810p-2f,-0x1.30e7fep-2f}, 0x1.9f660ap-1f}, //     9  
  {{-0x1.304b56p-2f,-0x1.304b44p-2f}, 0x1.9f7608p-1f}, //     9  
  {{-0x1.2fae74p-2f,-0x1.2fae62p-2f}, 0x1.9f8608p-1f}, //     9  
  {{-0x1.2f117ep-2f,-0x1.2f116cp-2f}, 0x1.9f9608p-1f}, //     9  
  {{-0x1.2e748ap-2f,-0x1.2e7478p-2f}, 0x1.9fa606p-1f}, //     9  
  {{-0x1.2dd782p-2f,-0x1.2dd770p-2f}, 0x1.9fb604p-1f}, //     9  
  {{-0x1.2d3a52p-2f,-0x1.2d3a40p-2f}, 0x1.9fc604p-1f}, //     9  
  {{-0x1.2c9d0ep-2f,-0x1.2c9cfcp-2f}, 0x1.9fd604p-1f}, //     9  
  {{-0x1.2bffccp-2f,-0x1.2bffbap-2f}, 0x1.9fe602p-1f}, //     9  
  {{-0x1.2b6276p-2f,-0x1.2b6264p-2f}, 0x1.9ff600p-1f}, //     9  
  {{-0x1.2ac4f8p-2f,-0x1.2ac4e6p-2f}, 0x1.a00600p-1f}, //     9  
  {{-0x1.2a2766p-2f,-0x1.2a2754p-2f}, 0x1.a01600p-1f}, //     9  
  {{-0x1.2989d6p-2f,-0x1.2989c4p-2f}, 0x1.a025fep-1f}, //     9  
  {{-0x1.28ec32p-2f,-0x1.28ec20p-2f}, 0x1.a035fcp-1f}, //     9  
  {{-0x1.284e66p-2f,-0x1.284e54p-2f}, 0x1.a045fcp-1f}, //     9  
  {{-0x1.27b086p-2f,-0x1.27b074p-2f}, 0x1.a055fcp-1f}, //     9  
  {{-0x1.2712a8p-2f,-0x1.271296p-2f}, 0x1.a065fap-1f}, //     9  
  {{-0x1.2674b6p-2f,-0x1.2674a4p-2f}, 0x1.a075f8p-1f}, //     9  
  {{-0x1.25d69cp-2f,-0x1.25d68ap-2f}, 0x1.a085f8p-1f}, //     9  
  {{-0x1.25386ep-2f,-0x1.25385cp-2f}, 0x1.a095f8p-1f}, //     9  
  {{-0x1.249a42p-2f,-0x1.249a30p-2f}, 0x1.a0a5f6p-1f}, //     9  
  {{-0x1.23fc02p-2f,-0x1.23fbf0p-2f}, 0x1.a0b5f4p-1f}, //     9  
  {{-0x1.235d9ap-2f,-0x1.235d88p-2f}, 0x1.a0c5f4p-1f}, //     9  
  {{-0x1.22bf1ep-2f,-0x1.22bf0cp-2f}, 0x1.a0d5f4p-1f}, //     9  
  {{-0x1.2220a4p-2f,-0x1.222092p-2f}, 0x1.a0e5f2p-1f}, //     9  
  {{-0x1.218216p-2f,-0x1.218204p-2f}, 0x1.a0f5f0p-1f}, //     9  
  {{-0x1.20e360p-2f,-0x1.20e34ep-2f}, 0x1.a105f0p-1f}, //     9  
  {{-0x1.204496p-2f,-0x1.204484p-2f}, 0x1.a115f0p-1f}, //     9  
  {{-0x1.1fa5ccp-2f,-0x1.1fa5bap-2f}, 0x1.a125eep-1f}, //     9  
  {{-0x1.1f06f0p-2f,-0x1.1f06dep-2f}, 0x1.a135ecp-1f}, //     9  
  {{-0x1.1e67ecp-2f,-0x1.1e67dap-2f}, 0x1.a145ecp-1f}, //     9  
  {{-0x1.1dc8d4p-2f,-0x1.1dc8c2p-2f}, 0x1.a155ecp-1f}, //     9  
  {{-0x1.1d29bep-2f,-0x1.1d29acp-2f}, 0x1.a165eap-1f}, //     9  
  {{-0x1.1c8a92p-2f,-0x1.1c8a80p-2f}, 0x1.a175e8p-1f}, //     9  
  {{-0x1.1beb40p-2f,-0x1.1beb2ep-2f}, 0x1.a185e8p-1f}, //     9  
  {{-0x1.1b4bdap-2f,-0x1.1b4bc8p-2f}, 0x1.a195e8p-1f}, //     9  
  {{-0x1.1aac74p-2f,-0x1.1aac62p-2f}, 0x1.a1a5e6p-1f}, //     9  
  {{-0x1.1a0cfcp-2f,-0x1.1a0ceap-2f}, 0x1.a1b5e4p-1f}, //     9  
  {{-0x1.196d5cp-2f,-0x1.196d4ap-2f}, 0x1.a1c5e4p-1f}, //     9  
  {{-0x1.18cda6p-2f,-0x1.18cd96p-2f}, 0x1.a1d5e4p-1f}, //     8  
  {{-0x1.182df4p-2f,-0x1.182de2p-2f}, 0x1.a1e5e2p-1f}, //     9  
  {{-0x1.178e2cp-2f,-0x1.178e1ap-2f}, 0x1.a1f5e0p-1f}, //     9  
  {{-0x1.16ee3ep-2f,-0x1.16ee2cp-2f}, 0x1.a205e0p-1f}, //     9  
  {{-0x1.164e3ap-2f,-0x1.164e28p-2f}, 0x1.a215e0p-1f}, //     9  
  {{-0x1.15ae38p-2f,-0x1.15ae26p-2f}, 0x1.a225dep-1f}, //     9  
  {{-0x1.150e22p-2f,-0x1.150e10p-2f}, 0x1.a235dcp-1f}, //     9  
  {{-0x1.146de6p-2f,-0x1.146dd4p-2f}, 0x1.a245dcp-1f}, //     9  
  {{-0x1.13cd94p-2f,-0x1.13cd82p-2f}, 0x1.a255dcp-1f}, //     9  
  {{-0x1.132d44p-2f,-0x1.132d32p-2f}, 0x1.a265dap-1f}, //     9  
  {{-0x1.128ce0p-2f,-0x1.128ccep-2f}, 0x1.a275d8p-1f}, //     9  
  {{-0x1.11ec54p-2f,-0x1.11ec42p-2f}, 0x1.a285d8p-1f}, //     9  
  {{-0x1.114bb6p-2f,-0x1.114ba4p-2f}, 0x1.a295d8p-1f}, //     9  
  {{-0x1.10ab16p-2f,-0x1.10ab04p-2f}, 0x1.a2a5d6p-1f}, //     9  
  {{-0x1.100a64p-2f,-0x1.100a52p-2f}, 0x1.a2b5d4p-1f}, //     9  
  {{-0x1.0f698ap-2f,-0x1.0f6978p-2f}, 0x1.a2c5d4p-1f}, //     9  
  {{-0x1.0ec89cp-2f,-0x1.0ec88ap-2f}, 0x1.a2d5d4p-1f}, //     9  
  {{-0x1.0e27aep-2f,-0x1.0e279cp-2f}, 0x1.a2e5d2p-1f}, //     9  
  {{-0x1.0d86aep-2f,-0x1.0d869cp-2f}, 0x1.a2f5d0p-1f}, //     9  
  {{-0x1.0ce586p-2f,-0x1.0ce572p-2f}, 0x1.a305d0p-1f}, //    10  
  {{-0x1.0c4448p-2f,-0x1.0c4436p-2f}, 0x1.a315d0p-1f}, //     9  
  {{-0x1.0ba30cp-2f,-0x1.0ba2fap-2f}, 0x1.a325cep-1f}, //     9  
  {{-0x1.0b01bep-2f,-0x1.0b01acp-2f}, 0x1.a335ccp-1f}, //     9  
  {{-0x1.0a6046p-2f,-0x1.0a6034p-2f}, 0x1.a345ccp-1f}, //     9  
  {{-0x1.09bebcp-2f,-0x1.09beaap-2f}, 0x1.a355ccp-1f}, //     9  
  {{-0x1.091d30p-2f,-0x1.091d1ep-2f}, 0x1.a365cap-1f}, //     9  
  {{-0x1.087b92p-2f,-0x1.087b80p-2f}, 0x1.a375c8p-1f}, //     9  
  {{-0x1.07d9cep-2f,-0x1.07d9bap-2f}, 0x1.a385c8p-1f}, //    10  
  {{-0x1.0737f4p-2f,-0x1.0737e2p-2f}, 0x1.a395c8p-1f}, //     9  
  {{-0x1.06961ap-2f,-0x1.069608p-2f}, 0x1.a3a5c6p-1f}, //     9  
  {{-0x1.05f42ep-2f,-0x1.05f41cp-2f}, 0x1.a3b5c4p-1f}, //     9  
  {{-0x1.05521ap-2f,-0x1.055208p-2f}, 0x1.a3c5c4p-1f}, //     9  
  {{-0x1.04aff2p-2f,-0x1.04afe0p-2f}, 0x1.a3d5c4p-1f}, //     9  
  {{-0x1.040dcap-2f,-0x1.040db8p-2f}, 0x1.a3e5c2p-1f}, //     9  
  {{-0x1.036b8ep-2f,-0x1.036b7cp-2f}, 0x1.a3f5c0p-1f}, //     9  
  {{-0x1.02c92cp-2f,-0x1.02c918p-2f}, 0x1.a405c0p-1f}, //    10  
  {{-0x1.0226b4p-2f,-0x1.0226a2p-2f}, 0x1.a415c0p-1f}, //     9  
  {{-0x1.01843ep-2f,-0x1.01842cp-2f}, 0x1.a425bep-1f}, //     9  
  {{-0x1.00e1b4p-2f,-0x1.00e1a2p-2f}, 0x1.a435bcp-1f}, //     9  
  {{-0x1.003f02p-2f,-0x1.003ef0p-2f}, 0x1.a445bcp-1f}, //     9  
  {{-0x1.ff387ap-3f,-0x1.ff3854p-3f}, 0x1.a455bcp-1f}, //    19  
  {{-0x1.fdf2f0p-3f,-0x1.fdf2cap-3f}, 0x1.a465bap-1f}, //    19  
  {{-0x1.fcad3ep-3f,-0x1.fcad18p-3f}, 0x1.a475b8p-1f}, //    19  
  {{-0x1.fb673cp-3f,-0x1.fb6716p-3f}, 0x1.a485b8p-1f}, //    19  
  {{-0x1.fa2114p-3f,-0x1.fa20eep-3f}, 0x1.a495b8p-1f}, //    19  
  {{-0x1.f8daecp-3f,-0x1.f8dac6p-3f}, 0x1.a4a5b6p-1f}, //    19  
  {{-0x1.f7949ep-3f,-0x1.f79476p-3f}, 0x1.a4b5b4p-1f}, //    20  
  {{-0x1.f64dfep-3f,-0x1.f64dd8p-3f}, 0x1.a4c5b4p-1f}, //    19  
  {{-0x1.f50738p-3f,-0x1.f50710p-3f}, 0x1.a4d5b4p-1f}, //    20  
  {{-0x1.f3c072p-3f,-0x1.f3c04cp-3f}, 0x1.a4e5b2p-1f}, //    19  
  {{-0x1.f27986p-3f,-0x1.f2795ep-3f}, 0x1.a4f5b0p-1f}, //    20  
  {{-0x1.f13248p-3f,-0x1.f13222p-3f}, 0x1.a505b0p-1f}, //    19  
  {{-0x1.efeae4p-3f,-0x1.efeabcp-3f}, 0x1.a515b0p-1f}, //    20  
  {{-0x1.eea380p-3f,-0x1.eea35ap-3f}, 0x1.a525aep-1f}, //    19  
  {{-0x1.ed5bf6p-3f,-0x1.ed5bd0p-3f}, 0x1.a535acp-1f}, //    19  
  {{-0x1.ec141cp-3f,-0x1.ec13f4p-3f}, 0x1.a545acp-1f}, //    20  
  {{-0x1.eacc18p-3f,-0x1.eacbf2p-3f}, 0x1.a555acp-1f}, //    19  
  {{-0x1.e98418p-3f,-0x1.e983f0p-3f}, 0x1.a565aap-1f}, //    20  
  {{-0x1.e83bf0p-3f,-0x1.e83bc8p-3f}, 0x1.a575a8p-1f}, //    20  
  {{-0x1.e6f376p-3f,-0x1.e6f350p-3f}, 0x1.a585a8p-1f}, //    19  
  {{-0x1.e5aad6p-3f,-0x1.e5aab0p-3f}, 0x1.a595a8p-1f}, //    19  
  {{-0x1.e46238p-3f,-0x1.e46210p-3f}, 0x1.a5a5a6p-1f}, //    20  
  {{-0x1.e31972p-3f,-0x1.e3194ap-3f}, 0x1.a5b5a4p-1f}, //    20  
  {{-0x1.e1d05ap-3f,-0x1.e1d034p-3f}, 0x1.a5c5a4p-1f}, //    19  
  {{-0x1.e0871cp-3f,-0x1.e086f4p-3f}, 0x1.a5d5a4p-1f}, //    20  
  {{-0x1.df3ddep-3f,-0x1.df3db8p-3f}, 0x1.a5e5a2p-1f}, //    19  
  {{-0x1.ddf47ap-3f,-0x1.ddf454p-3f}, 0x1.a5f5a0p-1f}, //    19  
  {{-0x1.dcaac6p-3f,-0x1.dcaa9ep-3f}, 0x1.a605a0p-1f}, //    20  
  {{-0x1.db60e8p-3f,-0x1.db60c2p-3f}, 0x1.a615a0p-1f}, //    19  
  {{-0x1.da170ep-3f,-0x1.da16e6p-3f}, 0x1.a6259ep-1f}, //    20  
  {{-0x1.d8cd0cp-3f,-0x1.d8cce4p-3f}, 0x1.a6359cp-1f}, //    20  
  {{-0x1.d782b8p-3f,-0x1.d78290p-3f}, 0x1.a6459cp-1f}, //    20  
  {{-0x1.d6383cp-3f,-0x1.d63816p-3f}, 0x1.a6559cp-1f}, //    19  
  {{-0x1.d4edc4p-3f,-0x1.d4ed9cp-3f}, 0x1.a6659ap-1f}, //    20  
  {{-0x1.d3a322p-3f,-0x1.d3a2fcp-3f}, 0x1.a67598p-1f}, //    19  
  {{-0x1.d25832p-3f,-0x1.d2580ap-3f}, 0x1.a68598p-1f}, //    20  
  {{-0x1.d10d18p-3f,-0x1.d10cf0p-3f}, 0x1.a69598p-1f}, //    20  
  {{-0x1.cfc200p-3f,-0x1.cfc1d8p-3f}, 0x1.a6a596p-1f}, //    20  
  {{-0x1.ce76c2p-3f,-0x1.ce769ap-3f}, 0x1.a6b594p-1f}, //    20  
  {{-0x1.cd2b32p-3f,-0x1.cd2b0ap-3f}, 0x1.a6c594p-1f}, //    20  
  {{-0x1.cbdf7ap-3f,-0x1.cbdf52p-3f}, 0x1.a6d594p-1f}, //    20  
  {{-0x1.ca93c4p-3f,-0x1.ca939cp-3f}, 0x1.a6e592p-1f}, //    20  
  {{-0x1.c947e6p-3f,-0x1.c947bep-3f}, 0x1.a6f590p-1f}, //    20  
  {{-0x1.c7fbb8p-3f,-0x1.c7fb90p-3f}, 0x1.a70590p-1f}, //    20  
  {{-0x1.c6af60p-3f,-0x1.c6af3ap-3f}, 0x1.a71590p-1f}, //    19  
  {{-0x1.c5630cp-3f,-0x1.c562e4p-3f}, 0x1.a7258ep-1f}, //    20  
  {{-0x1.c41690p-3f,-0x1.c41668p-3f}, 0x1.a7358cp-1f}, //    20  
  {{-0x1.c2c9c2p-3f,-0x1.c2c99cp-3f}, 0x1.a7458cp-1f}, //    19  
  {{-0x1.c17ccep-3f,-0x1.c17ca6p-3f}, 0x1.a7558cp-1f}, //    20  
  {{-0x1.c02fdap-3f,-0x1.c02fb4p-3f}, 0x1.a7658ap-1f}, //    19  
  {{-0x1.bee2c0p-3f,-0x1.bee298p-3f}, 0x1.a77588p-1f}, //    20  
  {{-0x1.bd9554p-3f,-0x1.bd952cp-3f}, 0x1.a78588p-1f}, //    20  
  {{-0x1.bc47c0p-3f,-0x1.bc4798p-3f}, 0x1.a79588p-1f}, //    20  
  {{-0x1.bafa2ep-3f,-0x1.bafa06p-3f}, 0x1.a7a586p-1f}, //    20  
  {{-0x1.b9ac76p-3f,-0x1.b9ac4ep-3f}, 0x1.a7b584p-1f}, //    20  
  {{-0x1.b85e6ap-3f,-0x1.b85e42p-3f}, 0x1.a7c584p-1f}, //    20  
  {{-0x1.b71038p-3f,-0x1.b71010p-3f}, 0x1.a7d584p-1f}, //    20  
  {{-0x1.b5c208p-3f,-0x1.b5c1e0p-3f}, 0x1.a7e582p-1f}, //    20  
  {{-0x1.b473aep-3f,-0x1.b47388p-3f}, 0x1.a7f580p-1f}, //    19  
  {{-0x1.b32506p-3f,-0x1.b324dep-3f}, 0x1.a80580p-1f}, //    20  
  {{-0x1.b1d634p-3f,-0x1.b1d60cp-3f}, 0x1.a81580p-1f}, //    20  
  {{-0x1.b08764p-3f,-0x1.b0873cp-3f}, 0x1.a8257ep-1f}, //    20  
  {{-0x1.af386cp-3f,-0x1.af3846p-3f}, 0x1.a8357cp-1f}, //    19  
  {{-0x1.ade924p-3f,-0x1.ade8fcp-3f}, 0x1.a8457cp-1f}, //    20  
  {{-0x1.ac99b4p-3f,-0x1.ac998cp-3f}, 0x1.a8557cp-1f}, //    20  
  {{-0x1.ab4a46p-3f,-0x1.ab4a1ep-3f}, 0x1.a8657ap-1f}, //    20  
  {{-0x1.a9fab0p-3f,-0x1.a9fa88p-3f}, 0x1.a87578p-1f}, //    20  
  {{-0x1.a8aac8p-3f,-0x1.a8aaa0p-3f}, 0x1.a88578p-1f}, //    20  
  {{-0x1.a75ab8p-3f,-0x1.a75a90p-3f}, 0x1.a89578p-1f}, //    20  
  {{-0x1.a60aaap-3f,-0x1.a60a82p-3f}, 0x1.a8a576p-1f}, //    20  
  {{-0x1.a4ba74p-3f,-0x1.a4ba4cp-3f}, 0x1.a8b574p-1f}, //    20  
  {{-0x1.a369eep-3f,-0x1.a369c6p-3f}, 0x1.a8c574p-1f}, //    20  
  {{-0x1.a2193ep-3f,-0x1.a21916p-3f}, 0x1.a8d574p-1f}, //    20  
  {{-0x1.a0c892p-3f,-0x1.a0c86ap-3f}, 0x1.a8e572p-1f}, //    20  
  {{-0x1.9f77bep-3f,-0x1.9f7796p-3f}, 0x1.a8f570p-1f}, //    20  
  {{-0x1.9e2698p-3f,-0x1.9e2670p-3f}, 0x1.a90570p-1f}, //    20  
  {{-0x1.9cd54ap-3f,-0x1.9cd522p-3f}, 0x1.a91570p-1f}, //    20  
  {{-0x1.9b83fep-3f,-0x1.9b83d6p-3f}, 0x1.a9256ep-1f}, //    20  
  {{-0x1.9a328ap-3f,-0x1.9a3262p-3f}, 0x1.a9356cp-1f}, //    20  
  {{-0x1.98e0c4p-3f,-0x1.98e09cp-3f}, 0x1.a9456cp-1f}, //    20  
  {{-0x1.978ed6p-3f,-0x1.978eaep-3f}, 0x1.a9556cp-1f}, //    20  
  {{-0x1.963cecp-3f,-0x1.963cc2p-3f}, 0x1.a9656ap-1f}, //    21  
  {{-0x1.94ead8p-3f,-0x1.94eab0p-3f}, 0x1.a97568p-1f}, //    20  
  {{-0x1.939872p-3f,-0x1.93984ap-3f}, 0x1.a98568p-1f}, //    20  
  {{-0x1.9245e6p-3f,-0x1.9245bep-3f}, 0x1.a99568p-1f}, //    20  
  {{-0x1.90f35cp-3f,-0x1.90f332p-3f}, 0x1.a9a566p-1f}, //    21  
  {{-0x1.8fa0a8p-3f,-0x1.8fa080p-3f}, 0x1.a9b564p-1f}, //    20  
  {{-0x1.8e4da4p-3f,-0x1.8e4d7cp-3f}, 0x1.a9c564p-1f}, //    20  
  {{-0x1.8cfa78p-3f,-0x1.8cfa4ep-3f}, 0x1.a9d564p-1f}, //    21  
  {{-0x1.8ba74ep-3f,-0x1.8ba724p-3f}, 0x1.a9e562p-1f}, //    21  
  {{-0x1.8a53fap-3f,-0x1.8a53d2p-3f}, 0x1.a9f560p-1f}, //    20  
  {{-0x1.890056p-3f,-0x1.89002ep-3f}, 0x1.aa0560p-1f}, //    20  
  {{-0x1.87ac8ap-3f,-0x1.87ac62p-3f}, 0x1.aa1560p-1f}, //    20  
  {{-0x1.8658c0p-3f,-0x1.865898p-3f}, 0x1.aa255ep-1f}, //    20  
  {{-0x1.8504cep-3f,-0x1.8504a6p-3f}, 0x1.aa355cp-1f}, //    20  
  {{-0x1.83b08ap-3f,-0x1.83b062p-3f}, 0x1.aa455cp-1f}, //    20  
  {{-0x1.825c1ep-3f,-0x1.825bf6p-3f}, 0x1.aa555cp-1f}, //    20  
  {{-0x1.8107b4p-3f,-0x1.81078cp-3f}, 0x1.aa655ap-1f}, //    20  
  {{-0x1.7fb324p-3f,-0x1.7fb2fap-3f}, 0x1.aa7558p-1f}, //    21  
  {{-0x1.7e5e40p-3f,-0x1.7e5e16p-3f}, 0x1.aa8558p-1f}, //    21  
  {{-0x1.7d0934p-3f,-0x1.7d090ap-3f}, 0x1.aa9558p-1f}, //    21  
  {{-0x1.7bb42ap-3f,-0x1.7bb402p-3f}, 0x1.aaa556p-1f}, //    20  
  {{-0x1.7a5ef8p-3f,-0x1.7a5ed0p-3f}, 0x1.aab554p-1f}, //    20  
  {{-0x1.790974p-3f,-0x1.79094cp-3f}, 0x1.aac554p-1f}, //    20  
  {{-0x1.77b3c8p-3f,-0x1.77b3a0p-3f}, 0x1.aad554p-1f}, //    20  
  {{-0x1.765e20p-3f,-0x1.765df6p-3f}, 0x1.aae552p-1f}, //    21  
  {{-0x1.75084ep-3f,-0x1.750826p-3f}, 0x1.aaf550p-1f}, //    20  
  {{-0x1.73b22ap-3f,-0x1.73b202p-3f}, 0x1.ab0550p-1f}, //    20  
  {{-0x1.725bdep-3f,-0x1.725bb6p-3f}, 0x1.ab1550p-1f}, //    20  
  {{-0x1.710594p-3f,-0x1.71056cp-3f}, 0x1.ab254ep-1f}, //    20  
  {{-0x1.6faf24p-3f,-0x1.6faefap-3f}, 0x1.ab354cp-1f}, //    21  
  {{-0x1.6e5860p-3f,-0x1.6e5836p-3f}, 0x1.ab454cp-1f}, //    21  
  {{-0x1.6d0174p-3f,-0x1.6d014ap-3f}, 0x1.ab554cp-1f}, //    21  
  {{-0x1.6baa8ap-3f,-0x1.6baa60p-3f}, 0x1.ab654ap-1f}, //    21  
  {{-0x1.6a5378p-3f,-0x1.6a5350p-3f}, 0x1.ab7548p-1f}, //    20  
  {{-0x1.68fc14p-3f,-0x1.68fbeap-3f}, 0x1.ab8548p-1f}, //    21  
  {{-0x1.67a488p-3f,-0x1.67a45ep-3f}, 0x1.ab9548p-1f}, //    21  
  {{-0x1.664cfep-3f,-0x1.664cd4p-3f}, 0x1.aba546p-1f}, //    21  
  {{-0x1.64f54cp-3f,-0x1.64f524p-3f}, 0x1.abb544p-1f}, //    20  
  {{-0x1.639d48p-3f,-0x1.639d1ep-3f}, 0x1.abc544p-1f}, //    21  
  {{-0x1.62451ap-3f,-0x1.6244f2p-3f}, 0x1.abd544p-1f}, //    20  
  {{-0x1.60ecf0p-3f,-0x1.60ecc8p-3f}, 0x1.abe542p-1f}, //    20  
  {{-0x1.5f94a0p-3f,-0x1.5f9476p-3f}, 0x1.abf540p-1f}, //    21  
  {{-0x1.5e3bfap-3f,-0x1.5e3bd0p-3f}, 0x1.ac0540p-1f}, //    21  
  {{-0x1.5ce32cp-3f,-0x1.5ce304p-3f}, 0x1.ac1540p-1f}, //    20  
  {{-0x1.5b8a62p-3f,-0x1.5b8a3ap-3f}, 0x1.ac253ep-1f}, //    20  
  {{-0x1.5a3170p-3f,-0x1.5a3148p-3f}, 0x1.ac353cp-1f}, //    20  
  {{-0x1.58d82ap-3f,-0x1.58d802p-3f}, 0x1.ac453cp-1f}, //    20  
  {{-0x1.577ebcp-3f,-0x1.577e94p-3f}, 0x1.ac553cp-1f}, //    20  
  {{-0x1.562552p-3f,-0x1.562528p-3f}, 0x1.ac653ap-1f}, //    21  
  {{-0x1.54cbc0p-3f,-0x1.54cb96p-3f}, 0x1.ac7538p-1f}, //    21  
  {{-0x1.5371dap-3f,-0x1.5371b0p-3f}, 0x1.ac8538p-1f}, //    21  
  {{-0x1.5217cap-3f,-0x1.5217a2p-3f}, 0x1.ac9538p-1f}, //    20  
  {{-0x1.50bdc0p-3f,-0x1.50bd96p-3f}, 0x1.aca536p-1f}, //    21  
  {{-0x1.4f638cp-3f,-0x1.4f6362p-3f}, 0x1.acb534p-1f}, //    21  
  {{-0x1.4e0906p-3f,-0x1.4e08dcp-3f}, 0x1.acc534p-1f}, //    21  
  {{-0x1.4cae56p-3f,-0x1.4cae2cp-3f}, 0x1.acd534p-1f}, //    21  
  {{-0x1.4b53aap-3f,-0x1.4b5380p-3f}, 0x1.ace532p-1f}, //    21  
  {{-0x1.49f8d6p-3f,-0x1.49f8acp-3f}, 0x1.acf530p-1f}, //    21  
  {{-0x1.489daep-3f,-0x1.489d86p-3f}, 0x1.ad0530p-1f}, //    20  
  {{-0x1.47425ep-3f,-0x1.474236p-3f}, 0x1.ad1530p-1f}, //    20  
  {{-0x1.45e712p-3f,-0x1.45e6e8p-3f}, 0x1.ad252ep-1f}, //    21  
  {{-0x1.448b9cp-3f,-0x1.448b74p-3f}, 0x1.ad352cp-1f}, //    20  
  {{-0x1.432fd4p-3f,-0x1.432faap-3f}, 0x1.ad452cp-1f}, //    21  
  {{-0x1.41d3e4p-3f,-0x1.41d3bap-3f}, 0x1.ad552cp-1f}, //    21  
  {{-0x1.4077f6p-3f,-0x1.4077ccp-3f}, 0x1.ad652ap-1f}, //    21  
  {{-0x1.3f1be0p-3f,-0x1.3f1bb6p-3f}, 0x1.ad7528p-1f}, //    21  
  {{-0x1.3dbf76p-3f,-0x1.3dbf4ep-3f}, 0x1.ad8528p-1f}, //    20  
  {{-0x1.3c62e6p-3f,-0x1.3c62bcp-3f}, 0x1.ad9528p-1f}, //    21  
  {{-0x1.3b0656p-3f,-0x1.3b062ep-3f}, 0x1.ada526p-1f}, //    20  
  {{-0x1.39a9a0p-3f,-0x1.39a976p-3f}, 0x1.adb524p-1f}, //    21  
  {{-0x1.384c96p-3f,-0x1.384c6cp-3f}, 0x1.adc524p-1f}, //    21  
  {{-0x1.36ef62p-3f,-0x1.36ef38p-3f}, 0x1.add524p-1f}, //    21  
  {{-0x1.359232p-3f,-0x1.35920ap-3f}, 0x1.ade522p-1f}, //    20  
  {{-0x1.3434dcp-3f,-0x1.3434b2p-3f}, 0x1.adf520p-1f}, //    21  
  {{-0x1.32d730p-3f,-0x1.32d706p-3f}, 0x1.ae0520p-1f}, //    21  
  {{-0x1.31795cp-3f,-0x1.317932p-3f}, 0x1.ae1520p-1f}, //    21  
  {{-0x1.301b8ap-3f,-0x1.301b62p-3f}, 0x1.ae251ep-1f}, //    20  
  {{-0x1.2ebd92p-3f,-0x1.2ebd68p-3f}, 0x1.ae351cp-1f}, //    21  
  {{-0x1.2d5f44p-3f,-0x1.2d5f1cp-3f}, 0x1.ae451cp-1f}, //    20  
  {{-0x1.2c00d0p-3f,-0x1.2c00a6p-3f}, 0x1.ae551cp-1f}, //    21  
  {{-0x1.2aa25ep-3f,-0x1.2aa234p-3f}, 0x1.ae651ap-1f}, //    21  
  {{-0x1.2943c4p-3f,-0x1.29439ap-3f}, 0x1.ae7518p-1f}, //    21  
  {{-0x1.27e4d6p-3f,-0x1.27e4acp-3f}, 0x1.ae8518p-1f}, //    21  
  {{-0x1.2685bep-3f,-0x1.268594p-3f}, 0x1.ae9518p-1f}, //    21  
  {{-0x1.2526acp-3f,-0x1.252682p-3f}, 0x1.aea516p-1f}, //    21  
  {{-0x1.23c770p-3f,-0x1.23c746p-3f}, 0x1.aeb514p-1f}, //    21  
  {{-0x1.2267e0p-3f,-0x1.2267b6p-3f}, 0x1.aec514p-1f}, //    21  
  {{-0x1.210828p-3f,-0x1.2107fep-3f}, 0x1.aed514p-1f}, //    21  
  {{-0x1.1fa874p-3f,-0x1.1fa84ap-3f}, 0x1.aee512p-1f}, //    21  
  {{-0x1.1e4896p-3f,-0x1.1e486cp-3f}, 0x1.aef510p-1f}, //    21  
  {{-0x1.1ce866p-3f,-0x1.1ce83cp-3f}, 0x1.af0510p-1f}, //    21  
  {{-0x1.1b880cp-3f,-0x1.1b87e2p-3f}, 0x1.af1510p-1f}, //    21  
  {{-0x1.1a27b6p-3f,-0x1.1a278cp-3f}, 0x1.af250ep-1f}, //    21  
  {{-0x1.18c736p-3f,-0x1.18c70cp-3f}, 0x1.af350cp-1f}, //    21  
  {{-0x1.176664p-3f,-0x1.17663ap-3f}, 0x1.af450cp-1f}, //    21  
  {{-0x1.160568p-3f,-0x1.16053ep-3f}, 0x1.af550cp-1f}, //    21  
  {{-0x1.14a470p-3f,-0x1.14a446p-3f}, 0x1.af650ap-1f}, //    21  
  {{-0x1.134350p-3f,-0x1.134326p-3f}, 0x1.af7508p-1f}, //    21  
  {{-0x1.11e1dcp-3f,-0x1.11e1b2p-3f}, 0x1.af8508p-1f}, //    21  
  {{-0x1.10803ep-3f,-0x1.108014p-3f}, 0x1.af9508p-1f}, //    21  
  {{-0x1.0f1ea6p-3f,-0x1.0f1e7cp-3f}, 0x1.afa506p-1f}, //    21  
  {{-0x1.0dbce4p-3f,-0x1.0dbcbap-3f}, 0x1.afb504p-1f}, //    21  
  {{-0x1.0c5acep-3f,-0x1.0c5aa4p-3f}, 0x1.afc504p-1f}, //    21  
  {{-0x1.0af88ep-3f,-0x1.0af864p-3f}, 0x1.afd504p-1f}, //    21  
  {{-0x1.099654p-3f,-0x1.099628p-3f}, 0x1.afe502p-1f}, //    22  
  {{-0x1.0833f0p-3f,-0x1.0833c6p-3f}, 0x1.aff500p-1f}, //    21  
  {{-0x1.06d138p-3f,-0x1.06d10cp-3f}, 0x1.b00500p-1f}, //    22  
  {{-0x1.056e56p-3f,-0x1.056e2cp-3f}, 0x1.b01500p-1f}, //    21  
  {{-0x1.040b7ap-3f,-0x1.040b4ep-3f}, 0x1.b024fep-1f}, //    22  
  {{-0x1.02a874p-3f,-0x1.02a84ap-3f}, 0x1.b034fcp-1f}, //    21  
  {{-0x1.01451ap-3f,-0x1.0144eep-3f}, 0x1.b044fcp-1f}, //    22  
  {{-0x1.ffc32ep-4f,-0x1.ffc2d8p-4f}, 0x1.b054fcp-1f}, //    43  
  {{-0x1.fcfc30p-4f,-0x1.fcfbdap-4f}, 0x1.b064fap-1f}, //    43  
  {{-0x1.fa34e0p-4f,-0x1.fa348ap-4f}, 0x1.b074f8p-1f}, //    43  
  {{-0x1.f76ce8p-4f,-0x1.f76c90p-4f}, 0x1.b084f8p-1f}, //    44  
  {{-0x1.f4a49ep-4f,-0x1.f4a446p-4f}, 0x1.b094f8p-1f}, //    44  
  {{-0x1.f1dc5cp-4f,-0x1.f1dc04p-4f}, 0x1.b0a4f6p-1f}, //    44  
  {{-0x1.ef13c8p-4f,-0x1.ef1372p-4f}, 0x1.b0b4f4p-1f}, //    43  
  {{-0x1.ec4a8cp-4f,-0x1.ec4a34p-4f}, 0x1.b0c4f4p-1f}, //    44  
  {{-0x1.e980fcp-4f,-0x1.e980a6p-4f}, 0x1.b0d4f4p-1f}, //    43  
  {{-0x1.e6b776p-4f,-0x1.e6b720p-4f}, 0x1.b0e4f2p-1f}, //    43  
  {{-0x1.e3ed9ep-4f,-0x1.e3ed48p-4f}, 0x1.b0f4f0p-1f}, //    43  
  {{-0x1.e1231cp-4f,-0x1.e122c6p-4f}, 0x1.b104f0p-1f}, //    43  
  {{-0x1.de584ap-4f,-0x1.de57f2p-4f}, 0x1.b114f0p-1f}, //    44  
  {{-0x1.db8d7ep-4f,-0x1.db8d28p-4f}, 0x1.b124eep-1f}, //    43  
  {{-0x1.d8c262p-4f,-0x1.d8c20cp-4f}, 0x1.b134ecp-1f}, //    43  
  {{-0x1.d5f69cp-4f,-0x1.d5f644p-4f}, 0x1.b144ecp-1f}, //    44  
  {{-0x1.d32a84p-4f,-0x1.d32a2cp-4f}, 0x1.b154ecp-1f}, //    44  
  {{-0x1.d05e74p-4f,-0x1.d05e1cp-4f}, 0x1.b164eap-1f}, //    44  
  {{-0x1.cd9214p-4f,-0x1.cd91bcp-4f}, 0x1.b174e8p-1f}, //    44  
  {{-0x1.cac508p-4f,-0x1.cac4b0p-4f}, 0x1.b184e8p-1f}, //    44  
  {{-0x1.c7f7acp-4f,-0x1.c7f754p-4f}, 0x1.b194e8p-1f}, //    44  
  {{-0x1.c52a56p-4f,-0x1.c529fep-4f}, 0x1.b1a4e6p-1f}, //    44  
  {{-0x1.c25cb0p-4f,-0x1.c25c5ap-4f}, 0x1.b1b4e4p-1f}, //    43  
  {{-0x1.bf8e60p-4f,-0x1.bf8e08p-4f}, 0x1.b1c4e4p-1f}, //    44  
  {{-0x1.bcbfbep-4f,-0x1.bcbf66p-4f}, 0x1.b1d4e4p-1f}, //    44  
  {{-0x1.b9f124p-4f,-0x1.b9f0ccp-4f}, 0x1.b1e4e2p-1f}, //    44  
  {{-0x1.b7223ap-4f,-0x1.b721e2p-4f}, 0x1.b1f4e0p-1f}, //    44  
  {{-0x1.b452a4p-4f,-0x1.b4524cp-4f}, 0x1.b204e0p-1f}, //    44  
  {{-0x1.b182bcp-4f,-0x1.b18264p-4f}, 0x1.b214e0p-1f}, //    44  
  {{-0x1.aeb2dep-4f,-0x1.aeb286p-4f}, 0x1.b224dep-1f}, //    44  
  {{-0x1.abe2aep-4f,-0x1.abe256p-4f}, 0x1.b234dcp-1f}, //    44  
  {{-0x1.a911d2p-4f,-0x1.a9117ap-4f}, 0x1.b244dcp-1f}, //    44  
  {{-0x1.a640a4p-4f,-0x1.a6404cp-4f}, 0x1.b254dcp-1f}, //    44  
  {{-0x1.a36f80p-4f,-0x1.a36f28p-4f}, 0x1.b264dap-1f}, //    44  
  {{-0x1.a09e0ap-4f,-0x1.a09db2p-4f}, 0x1.b274d8p-1f}, //    44  
  {{-0x1.9dcbeap-4f,-0x1.9dcb92p-4f}, 0x1.b284d8p-1f}, //    44  
  {{-0x1.9af976p-4f,-0x1.9af91ep-4f}, 0x1.b294d8p-1f}, //    44  
  {{-0x1.98270cp-4f,-0x1.9826b4p-4f}, 0x1.b2a4d6p-1f}, //    44  
  {{-0x1.955452p-4f,-0x1.9553fap-4f}, 0x1.b2b4d4p-1f}, //    44  
  {{-0x1.9280eap-4f,-0x1.928092p-4f}, 0x1.b2c4d4p-1f}, //    44  
  {{-0x1.8fad32p-4f,-0x1.8facdap-4f}, 0x1.b2d4d4p-1f}, //    44  
  {{-0x1.8cd982p-4f,-0x1.8cd92ap-4f}, 0x1.b2e4d2p-1f}, //    44  
  {{-0x1.8a0580p-4f,-0x1.8a0528p-4f}, 0x1.b2f4d0p-1f}, //    44  
  {{-0x1.8730d4p-4f,-0x1.87307cp-4f}, 0x1.b304d0p-1f}, //    44  
  {{-0x1.845bd4p-4f,-0x1.845b7cp-4f}, 0x1.b314d0p-1f}, //    44  
  {{-0x1.8186e0p-4f,-0x1.818686p-4f}, 0x1.b324cep-1f}, //    45  
  {{-0x1.7eb198p-4f,-0x1.7eb140p-4f}, 0x1.b334ccp-1f}, //    44  
  {{-0x1.7bdba4p-4f,-0x1.7bdb4cp-4f}, 0x1.b344ccp-1f}, //    44  
  {{-0x1.790560p-4f,-0x1.790506p-4f}, 0x1.b354ccp-1f}, //    45  
  {{-0x1.762f24p-4f,-0x1.762ecap-4f}, 0x1.b364cap-1f}, //    45  
  {{-0x1.735896p-4f,-0x1.73583ep-4f}, 0x1.b374c8p-1f}, //    44  
  {{-0x1.70815cp-4f,-0x1.708104p-4f}, 0x1.b384c8p-1f}, //    44  
  {{-0x1.6da9d0p-4f,-0x1.6da978p-4f}, 0x1.b394c8p-1f}, //    44  
  {{-0x1.6ad24ep-4f,-0x1.6ad1f6p-4f}, 0x1.b3a4c6p-1f}, //    44  
  {{-0x1.67fa7ap-4f,-0x1.67fa22p-4f}, 0x1.b3b4c4p-1f}, //    44  
  {{-0x1.6521fap-4f,-0x1.6521a0p-4f}, 0x1.b3c4c4p-1f}, //    45  
  {{-0x1.624928p-4f,-0x1.6248cep-4f}, 0x1.b3d4c4p-1f}, //    45  
  {{-0x1.5f705ep-4f,-0x1.5f7006p-4f}, 0x1.b3e4c2p-1f}, //    44  
  {{-0x1.5c9744p-4f,-0x1.5c96ecp-4f}, 0x1.b3f4c0p-1f}, //    44  
  {{-0x1.59bd7cp-4f,-0x1.59bd24p-4f}, 0x1.b404c0p-1f}, //    44  
  {{-0x1.56e364p-4f,-0x1.56e30ap-4f}, 0x1.b414c0p-1f}, //    45  
  {{-0x1.540954p-4f,-0x1.5408fap-4f}, 0x1.b424bep-1f}, //    45  
  {{-0x1.512ef2p-4f,-0x1.512e9ap-4f}, 0x1.b434bcp-1f}, //    44  
  {{-0x1.4e53e4p-4f,-0x1.4e538cp-4f}, 0x1.b444bcp-1f}, //    44  
  {{-0x1.4b7884p-4f,-0x1.4b782ap-4f}, 0x1.b454bcp-1f}, //    45  
  {{-0x1.489d2ep-4f,-0x1.489cd4p-4f}, 0x1.b464bap-1f}, //    45  
  {{-0x1.45c186p-4f,-0x1.45c12cp-4f}, 0x1.b474b8p-1f}, //    45  
  {{-0x1.42e530p-4f,-0x1.42e4d6p-4f}, 0x1.b484b8p-1f}, //    45  
  {{-0x1.400888p-4f,-0x1.400830p-4f}, 0x1.b494b8p-1f}, //    44  
  {{-0x1.3d2beap-4f,-0x1.3d2b92p-4f}, 0x1.b4a4b6p-1f}, //    44  
  {{-0x1.3a4efcp-4f,-0x1.3a4ea2p-4f}, 0x1.b4b4b4p-1f}, //    45  
  {{-0x1.37715ep-4f,-0x1.377104p-4f}, 0x1.b4c4b4p-1f}, //    45  
  {{-0x1.349370p-4f,-0x1.349316p-4f}, 0x1.b4d4b4p-1f}, //    45  
  {{-0x1.31b58ap-4f,-0x1.31b532p-4f}, 0x1.b4e4b2p-1f}, //    44  
  {{-0x1.2ed754p-4f,-0x1.2ed6fap-4f}, 0x1.b4f4b0p-1f}, //    45  
  {{-0x1.2bf870p-4f,-0x1.2bf816p-4f}, 0x1.b504b0p-1f}, //    45  
  {{-0x1.29193ap-4f,-0x1.2918e0p-4f}, 0x1.b514b0p-1f}, //    45  
  {{-0x1.263a0cp-4f,-0x1.2639b2p-4f}, 0x1.b524aep-1f}, //    45  
  {{-0x1.235a8ep-4f,-0x1.235a34p-4f}, 0x1.b534acp-1f}, //    45  
  {{-0x1.207a62p-4f,-0x1.207a08p-4f}, 0x1.b544acp-1f}, //    45  
  {{-0x1.1d99e4p-4f,-0x1.1d998ap-4f}, 0x1.b554acp-1f}, //    45  
  {{-0x1.1ab970p-4f,-0x1.1ab916p-4f}, 0x1.b564aap-1f}, //    45  
  {{-0x1.17d8aap-4f,-0x1.17d850p-4f}, 0x1.b574a8p-1f}, //    45  
  {{-0x1.14f736p-4f,-0x1.14f6dcp-4f}, 0x1.b584a8p-1f}, //    45  
  {{-0x1.121570p-4f,-0x1.121514p-4f}, 0x1.b594a8p-1f}, //    46  
  {{-0x1.0f33b4p-4f,-0x1.0f3358p-4f}, 0x1.b5a4a6p-1f}, //    46  
  {{-0x1.0c51a6p-4f,-0x1.0c514ap-4f}, 0x1.b5b4a4p-1f}, //    46  
  {{-0x1.096ee8p-4f,-0x1.096e8ep-4f}, 0x1.b5c4a4p-1f}, //    45  
  {{-0x1.068bdap-4f,-0x1.068b80p-4f}, 0x1.b5d4a4p-1f}, //    45  
  {{-0x1.03a8d6p-4f,-0x1.03a87cp-4f}, 0x1.b5e4a2p-1f}, //    45  
  {{-0x1.00c580p-4f,-0x1.00c526p-4f}, 0x1.b5f4a0p-1f}, //    45  
  {{-0x1.fbc2f8p-5f,-0x1.fbc240p-5f}, 0x1.b604a0p-1f}, //    92  
  {{-0x1.f5fa4ap-5f,-0x1.f5f994p-5f}, 0x1.b614a0p-1f}, //    91  
  {{-0x1.f031b2p-5f,-0x1.f030fap-5f}, 0x1.b6249ep-1f}, //    92  
  {{-0x1.ea6874p-5f,-0x1.ea67bep-5f}, 0x1.b6349cp-1f}, //    91  
  {{-0x1.e49ddap-5f,-0x1.e49d24p-5f}, 0x1.b6449cp-1f}, //    91  
  {{-0x1.ded29cp-5f,-0x1.ded1e4p-5f}, 0x1.b6549cp-1f}, //    92  
  {{-0x1.d90772p-5f,-0x1.d906bap-5f}, 0x1.b6649ap-1f}, //    92  
  {{-0x1.d33ba4p-5f,-0x1.d33aeep-5f}, 0x1.b67498p-1f}, //    91  
  {{-0x1.cd6e78p-5f,-0x1.cd6dc2p-5f}, 0x1.b68498p-1f}, //    91  
  {{-0x1.c7a0a8p-5f,-0x1.c79ff2p-5f}, 0x1.b69498p-1f}, //    91  
  {{-0x1.c1d2eep-5f,-0x1.c1d236p-5f}, 0x1.b6a496p-1f}, //    92  
  {{-0x1.bc048ep-5f,-0x1.bc03d8p-5f}, 0x1.b6b494p-1f}, //    91  
  {{-0x1.b634d2p-5f,-0x1.b6341ap-5f}, 0x1.b6c494p-1f}, //    92  
  {{-0x1.b06470p-5f,-0x1.b063b8p-5f}, 0x1.b6d494p-1f}, //    92  
  {{-0x1.aa9424p-5f,-0x1.aa936ap-5f}, 0x1.b6e492p-1f}, //    93  
  {{-0x1.a4c332p-5f,-0x1.a4c27ap-5f}, 0x1.b6f490p-1f}, //    92  
  {{-0x1.9ef0e2p-5f,-0x1.9ef02ap-5f}, 0x1.b70490p-1f}, //    92  
  {{-0x1.991deep-5f,-0x1.991d36p-5f}, 0x1.b71490p-1f}, //    92  
  {{-0x1.934b10p-5f,-0x1.934a58p-5f}, 0x1.b7248ep-1f}, //    92  
  {{-0x1.8d778ep-5f,-0x1.8d76d4p-5f}, 0x1.b7348cp-1f}, //    93  
  {{-0x1.87a2acp-5f,-0x1.87a1f2p-5f}, 0x1.b7448cp-1f}, //    93  
  {{-0x1.81cd24p-5f,-0x1.81cc6cp-5f}, 0x1.b7548cp-1f}, //    92  
  {{-0x1.7bf7b4p-5f,-0x1.7bf6fap-5f}, 0x1.b7648ap-1f}, //    93  
  {{-0x1.76219ep-5f,-0x1.7620e6p-5f}, 0x1.b77488p-1f}, //    92  
  {{-0x1.704a2ap-5f,-0x1.704970p-5f}, 0x1.b78488p-1f}, //    93  
  {{-0x1.6a7210p-5f,-0x1.6a7156p-5f}, 0x1.b79488p-1f}, //    93  
  {{-0x1.649a0cp-5f,-0x1.649952p-5f}, 0x1.b7a486p-1f}, //    93  
  {{-0x1.5ec164p-5f,-0x1.5ec0aap-5f}, 0x1.b7b484p-1f}, //    93  
  {{-0x1.58e75cp-5f,-0x1.58e6a2p-5f}, 0x1.b7c484p-1f}, //    93  
  {{-0x1.530caep-5f,-0x1.530bf4p-5f}, 0x1.b7d484p-1f}, //    93  
  {{-0x1.4d3218p-5f,-0x1.4d315ep-5f}, 0x1.b7e482p-1f}, //    93  
  {{-0x1.4756dcp-5f,-0x1.475622p-5f}, 0x1.b7f480p-1f}, //    93  
  {{-0x1.417a40p-5f,-0x1.417986p-5f}, 0x1.b80480p-1f}, //    93  
  {{-0x1.3b9d00p-5f,-0x1.3b9c46p-5f}, 0x1.b81480p-1f}, //    93  
  {{-0x1.35bfd6p-5f,-0x1.35bf1cp-5f}, 0x1.b8247ep-1f}, //    93  
  {{-0x1.2fe206p-5f,-0x1.2fe14cp-5f}, 0x1.b8347cp-1f}, //    93  
  {{-0x1.2a02d6p-5f,-0x1.2a021cp-5f}, 0x1.b8447cp-1f}, //    93  
  {{-0x1.242302p-5f,-0x1.242248p-5f}, 0x1.b8547cp-1f}, //    93  
  {{-0x1.1e4344p-5f,-0x1.1e428ap-5f}, 0x1.b8647ap-1f}, //    93  
  {{-0x1.1862e0p-5f,-0x1.186226p-5f}, 0x1.b87478p-1f}, //    93  
  {{-0x1.12811cp-5f,-0x1.128062p-5f}, 0x1.b88478p-1f}, //    93  
  {{-0x1.0c9eb2p-5f,-0x1.0c9df8p-5f}, 0x1.b89478p-1f}, //    93  
  {{-0x1.06bc60p-5f,-0x1.06bba6p-5f}, 0x1.b8a476p-1f}, //    93  
  {{-0x1.00d96ap-5f,-0x1.00d8aep-5f}, 0x1.b8b474p-1f}, //    94  
  {{-0x1.f5ea22p-6f,-0x1.f5e8aap-6f}, 0x1.b8c474p-1f}, //   188  
  {{-0x1.ea2024p-6f,-0x1.ea1eaep-6f}, 0x1.b8d474p-1f}, //   187  
  {{-0x1.de5656p-6f,-0x1.de54e0p-6f}, 0x1.b8e472p-1f}, //   187  
  {{-0x1.d28b3ep-6f,-0x1.d289c8p-6f}, 0x1.b8f470p-1f}, //   187  
  {{-0x1.c6bd62p-6f,-0x1.c6bbeap-6f}, 0x1.b90470p-1f}, //   188  
  {{-0x1.baee3cp-6f,-0x1.baecc4p-6f}, 0x1.b91470p-1f}, //   188  
  {{-0x1.af1f44p-6f,-0x1.af1dccp-6f}, 0x1.b9246ep-1f}, //   188  
  {{-0x1.a34f02p-6f,-0x1.a34d8ap-6f}, 0x1.b9346cp-1f}, //   188  
  {{-0x1.977bfap-6f,-0x1.977a82p-6f}, 0x1.b9446cp-1f}, //   188  
  {{-0x1.8ba7a8p-6f,-0x1.8ba630p-6f}, 0x1.b9546cp-1f}, //   188  
  {{-0x1.7fd386p-6f,-0x1.7fd20cp-6f}, 0x1.b9646ap-1f}, //   189  
  {{-0x1.73fe18p-6f,-0x1.73fca0p-6f}, 0x1.b97468p-1f}, //   188  
  {{-0x1.6825e6p-6f,-0x1.68246cp-6f}, 0x1.b98468p-1f}, //   189  
  {{-0x1.5c4c68p-6f,-0x1.5c4aeep-6f}, 0x1.b99468p-1f}, //   189  
  {{-0x1.50731ap-6f,-0x1.5071a0p-6f}, 0x1.b9a466p-1f}, //   189  
  {{-0x1.449880p-6f,-0x1.449706p-6f}, 0x1.b9b464p-1f}, //   189  
  {{-0x1.38bb20p-6f,-0x1.38b9a8p-6f}, 0x1.b9c464p-1f}, //   188  
  {{-0x1.2cdc76p-6f,-0x1.2cdafcp-6f}, 0x1.b9d464p-1f}, //   189  
  {{-0x1.20fdfcp-6f,-0x1.20fc82p-6f}, 0x1.b9e462p-1f}, //   189  
  {{-0x1.151e36p-6f,-0x1.151cbcp-6f}, 0x1.b9f460p-1f}, //   189  
  {{-0x1.093baap-6f,-0x1.093a30p-6f}, 0x1.ba0460p-1f}, //   189  
  {{-0x1.faafa4p-7f,-0x1.faacacp-7f}, 0x1.ba1460p-1f}, //   380  
  {{-0x1.e2e856p-7f,-0x1.e2e55ep-7f}, 0x1.ba245ep-1f}, //   380  
  {{-0x1.cb1e70p-7f,-0x1.cb1b78p-7f}, 0x1.ba345cp-1f}, //   380  
  {{-0x1.b34efap-7f,-0x1.b34c02p-7f}, 0x1.ba445cp-1f}, //   380  
  {{-0x1.9b7ceep-7f,-0x1.9b79f6p-7f}, 0x1.ba545cp-1f}, //   380  
  {{-0x1.83ab44p-7f,-0x1.83a84cp-7f}, 0x1.ba645ap-1f}, //   380  
  {{-0x1.6bd704p-7f,-0x1.6bd40ap-7f}, 0x1.ba7458p-1f}, //   381  
  {{-0x1.53fd30p-7f,-0x1.53fa36p-7f}, 0x1.ba8458p-1f}, //   381  
  {{-0x1.3c20c6p-7f,-0x1.3c1dccp-7f}, 0x1.ba9458p-1f}, //   381  
  {{-0x1.2444bep-7f,-0x1.2441c4p-7f}, 0x1.baa456p-1f}, //   381  
  {{-0x1.0c6620p-7f,-0x1.0c6326p-7f}, 0x1.bab454p-1f}, //   381  
  {{-0x1.e903dep-8f,-0x1.e8fde6p-8f}, 0x1.bac454p-1f}, //   764  
  {{-0x1.b93648p-8f,-0x1.b93050p-8f}, 0x1.bad454p-1f}, //   764  
  {{-0x1.89697ep-8f,-0x1.896386p-8f}, 0x1.bae452p-1f}, //   764  
  {{-0x1.599784p-8f,-0x1.59918cp-8f}, 0x1.baf450p-1f}, //   764  
  {{-0x1.29ba5ep-8f,-0x1.29b466p-8f}, 0x1.bb0450p-1f}, //   764  
  {{-0x1.f3b012p-9f,-0x1.f3a41ap-9f}, 0x1.bb1450p-1f}, //  1532  
  {{-0x1.93ecfep-9f,-0x1.93e104p-9f}, 0x1.bb244ep-1f}, //  1533  
  {{-0x1.341f88p-9f,-0x1.34138ep-9f}, 0x1.bb344cp-1f}, //  1533  
  {{-0x1.a8776cp-10f,-0x1.a85f74p-10f}, 0x1.bb444cp-1f}, //  3068  
  {{-0x1.d135fep-11f,-0x1.d10606p-11f}, 0x1.bb544cp-1f}, //  6140  
  {{-0x1.460e52p-13f,-0x1.454e58p-13f}, 0x1.bb644ap-1f}, // 24573  
  {{0x1.2e8860p-11f,0x1.2e585ep-11f}, 0x1.bb7448p-1f}, //  6145  
  {{0x1.575ef4p-10f,0x1.5746f2p-10f}, 0x1.bb8448p-1f}, //  3073  
  {{ 0x1.0bc740p-9f, 0x1.0bbb40p-9f}, 0x1.bb9448p-1f}, //  1536  
  {{ 0x1.6bdd68p-9f, 0x1.6bd166p-9f}, 0x1.bba446p-1f}, //  1537  
  {{ 0x1.cbfdf4p-9f, 0x1.cbf1f0p-9f}, 0x1.bbb444p-1f}, //  1538  
  {{ 0x1.161a74p-8f, 0x1.161472p-8f}, 0x1.bbc444p-1f}, //   769  
  {{ 0x1.463b22p-8f, 0x1.463520p-8f}, 0x1.bbd444p-1f}, //   769  
  {{ 0x1.765b00p-8f, 0x1.7654fcp-8f}, 0x1.bbe442p-1f}, //   770  
  {{ 0x1.a68010p-8f, 0x1.a67a0cp-8f}, 0x1.bbf440p-1f}, //   770  
  {{ 0x1.d6b058p-8f, 0x1.d6aa52p-8f}, 0x1.bc0440p-1f}, //   771  
  {{ 0x1.0372eap-7f, 0x1.036fe8p-7f}, 0x1.bc1440p-1f}, //   385  
  {{ 0x1.1b8d3ep-7f, 0x1.1b8a3cp-7f}, 0x1.bc243ep-1f}, //   385  
  {{ 0x1.33aa2cp-7f, 0x1.33a72ap-7f}, 0x1.bc343cp-1f}, //   385  
  {{ 0x1.4bccb8p-7f, 0x1.4bc9b6p-7f}, 0x1.bc443cp-1f}, //   385  
  {{ 0x1.63f1dep-7f, 0x1.63eedcp-7f}, 0x1.bc543cp-1f}, //   385  
  {{ 0x1.7c169ap-7f, 0x1.7c1396p-7f}, 0x1.bc643ap-1f}, //   386  
  {{ 0x1.943df0p-7f, 0x1.943aecp-7f}, 0x1.bc7438p-1f}, //   386  
  {{ 0x1.ac6ae6p-7f, 0x1.ac67e2p-7f}, 0x1.bc8438p-1f}, //   386  
  {{ 0x1.c49a76p-7f, 0x1.c49772p-7f}, 0x1.bc9438p-1f}, //   386  
  {{ 0x1.dcc99ap-7f, 0x1.dcc696p-7f}, 0x1.bca436p-1f}, //   386  
  {{ 0x1.f4fb5ap-7f, 0x1.f4f854p-7f}, 0x1.bcb434p-1f}, //   387  
  {{ 0x1.06995cp-6f, 0x1.0697dcp-6f}, 0x1.bcc434p-1f}, //   192  
  {{ 0x1.12b65ap-6f, 0x1.12b4d8p-6f}, 0x1.bcd434p-1f}, //   193  
  {{ 0x1.1ed322p-6f, 0x1.1ed1a0p-6f}, 0x1.bce432p-1f}, //   193  
  {{ 0x1.2af138p-6f, 0x1.2aefb6p-6f}, 0x1.bcf430p-1f}, //   193  
  {{ 0x1.37121ep-6f, 0x1.37109cp-6f}, 0x1.bd0430p-1f}, //   193  
  {{ 0x1.433452p-6f, 0x1.4332d0p-6f}, 0x1.bd1430p-1f}, //   193  
  {{ 0x1.4f5650p-6f, 0x1.4f54cep-6f}, 0x1.bd242ep-1f}, //   193  
  {{ 0x1.5b799ap-6f, 0x1.5b7818p-6f}, 0x1.bd342cp-1f}, //   193  
  {{ 0x1.679fb8p-6f, 0x1.679e36p-6f}, 0x1.bd442cp-1f}, //   193  
  {{ 0x1.73c724p-6f, 0x1.73c5a0p-6f}, 0x1.bd542cp-1f}, //   194  
  {{ 0x1.7fee58p-6f, 0x1.7fecd4p-6f}, 0x1.bd642ap-1f}, //   194  
  {{ 0x1.8c16dap-6f, 0x1.8c1556p-6f}, 0x1.bd7428p-1f}, //   194  
  {{ 0x1.984230p-6f, 0x1.9840acp-6f}, 0x1.bd8428p-1f}, //   194  
  {{ 0x1.a46ed2p-6f, 0x1.a46d50p-6f}, 0x1.bd9428p-1f}, //   193  
  {{ 0x1.b09b3ep-6f, 0x1.b099bap-6f}, 0x1.bda426p-1f}, //   194  
  {{ 0x1.bcc8f8p-6f, 0x1.bcc774p-6f}, 0x1.bdb424p-1f}, //   194  
  {{ 0x1.c8f986p-6f, 0x1.c8f802p-6f}, 0x1.bdc424p-1f}, //   194  
  {{ 0x1.d52b62p-6f, 0x1.d529dep-6f}, 0x1.bdd424p-1f}, //   194  
  {{ 0x1.e15d06p-6f, 0x1.e15b82p-6f}, 0x1.bde422p-1f}, //   194  
  {{ 0x1.ed8ff8p-6f, 0x1.ed8e74p-6f}, 0x1.bdf420p-1f}, //   194  
  {{ 0x1.f9c5c0p-6f, 0x1.f9c43ap-6f}, 0x1.be0420p-1f}, //   195  
  {{ 0x1.02fe6ap-5f, 0x1.02fda8p-5f}, 0x1.be1420p-1f}, //    97  
  {{ 0x1.0919d8p-5f, 0x1.091918p-5f}, 0x1.be241ep-1f}, //    96  
  {{ 0x1.0f35eep-5f, 0x1.0f352cp-5f}, 0x1.be341cp-1f}, //    97  
  {{ 0x1.15536ep-5f, 0x1.1552aep-5f}, 0x1.be441cp-1f}, //    96  
  {{ 0x1.1b7196p-5f, 0x1.1b70d4p-5f}, 0x1.be541cp-1f}, //    97  
  {{ 0x1.218fa2p-5f, 0x1.218ee0p-5f}, 0x1.be641ap-1f}, //    97  
  {{ 0x1.27ae54p-5f, 0x1.27ad92p-5f}, 0x1.be7418p-1f}, //    97  
  {{ 0x1.2dce72p-5f, 0x1.2dcdb0p-5f}, 0x1.be8418p-1f}, //    97  
  {{ 0x1.33ef38p-5f, 0x1.33ee76p-5f}, 0x1.be9418p-1f}, //    97  
  {{ 0x1.3a0fe0p-5f, 0x1.3a0f1ep-5f}, 0x1.bea416p-1f}, //    97  
  {{ 0x1.403130p-5f, 0x1.40306ep-5f}, 0x1.beb414p-1f}, //    97  
  {{ 0x1.4653ecp-5f, 0x1.46532ap-5f}, 0x1.bec414p-1f}, //    97  
  {{ 0x1.4c7750p-5f, 0x1.4c768cp-5f}, 0x1.bed414p-1f}, //    98  
  {{ 0x1.529a96p-5f, 0x1.5299d4p-5f}, 0x1.bee412p-1f}, //    97  
  {{ 0x1.58be84p-5f, 0x1.58bdc0p-5f}, 0x1.bef410p-1f}, //    98  
  {{ 0x1.5ee3dep-5f, 0x1.5ee31ap-5f}, 0x1.bf0410p-1f}, //    98  
  {{ 0x1.6509dep-5f, 0x1.65091cp-5f}, 0x1.bf1410p-1f}, //    97  
  {{ 0x1.6b2fc2p-5f, 0x1.6b2f00p-5f}, 0x1.bf240ep-1f}, //    97  
  {{ 0x1.71564ep-5f, 0x1.71558cp-5f}, 0x1.bf340cp-1f}, //    97  
  {{ 0x1.777e46p-5f, 0x1.777d84p-5f}, 0x1.bf440cp-1f}, //    97  
  {{ 0x1.7da6e6p-5f, 0x1.7da624p-5f}, 0x1.bf540cp-1f}, //    97  
  {{ 0x1.83cf6ap-5f, 0x1.83cea6p-5f}, 0x1.bf640ap-1f}, //    98  
  {{ 0x1.89f894p-5f, 0x1.89f7d0p-5f}, 0x1.bf7408p-1f}, //    98  
  {{ 0x1.90232ap-5f, 0x1.902268p-5f}, 0x1.bf8408p-1f}, //    97  
  {{ 0x1.964e6ap-5f, 0x1.964da6p-5f}, 0x1.bf9408p-1f}, //    98  
  {{ 0x1.9c798ap-5f, 0x1.9c78c8p-5f}, 0x1.bfa406p-1f}, //    97  
  {{ 0x1.a2a554p-5f, 0x1.a2a490p-5f}, 0x1.bfb404p-1f}, //    98  
  {{ 0x1.a8d28ap-5f, 0x1.a8d1c6p-5f}, 0x1.bfc404p-1f}, //    98  
  {{ 0x1.af0068p-5f, 0x1.aeffa4p-5f}, 0x1.bfd404p-1f}, //    98  
  {{ 0x1.b52e2ap-5f, 0x1.b52d66p-5f}, 0x1.bfe402p-1f}, //    98  
  {{ 0x1.bb5c92p-5f, 0x1.bb5bcep-5f}, 0x1.bff400p-1f}, //    98  
  {{ 0x1.c18c68p-5f, 0x1.c18ba4p-5f}, 0x1.c00400p-1f}, //    98  
  {{ 0x1.c7bce6p-5f, 0x1.c7bc22p-5f}, 0x1.c01400p-1f}, //    98  
  {{ 0x1.cded46p-5f, 0x1.cdec82p-5f}, 0x1.c023fep-1f}, //    98  
  {{ 0x1.d41e4cp-5f, 0x1.d41d88p-5f}, 0x1.c033fcp-1f}, //    98  
  {{ 0x1.da50c4p-5f, 0x1.da4ffep-5f}, 0x1.c043fcp-1f}, //    99  
  {{ 0x1.e083e2p-5f, 0x1.e0831cp-5f}, 0x1.c053fcp-1f}, //    99  
  {{ 0x1.e6b6e0p-5f, 0x1.e6b61cp-5f}, 0x1.c063fap-1f}, //    98  
  {{ 0x1.ecea88p-5f, 0x1.ece9c4p-5f}, 0x1.c073f8p-1f}, //    98  
  {{ 0x1.f31fa0p-5f, 0x1.f31edap-5f}, 0x1.c083f8p-1f}, //    99  
  {{ 0x1.f9555ep-5f, 0x1.f95498p-5f}, 0x1.c093f8p-1f}, //    99  
  {{ 0x1.ff8afep-5f, 0x1.ff8a38p-5f}, 0x1.c0a3f6p-1f}, //    99  
  {{ 0x1.02e0a2p-4f, 0x1.02e040p-4f}, 0x1.c0b3f4p-1f}, //    49  
  {{ 0x1.05fc7ep-4f, 0x1.05fc1cp-4f}, 0x1.c0c3f4p-1f}, //    49  
  {{ 0x1.0918aep-4f, 0x1.09184cp-4f}, 0x1.c0d3f4p-1f}, //    49  
  {{ 0x1.0c34cep-4f, 0x1.0c346cp-4f}, 0x1.c0e3f2p-1f}, //    49  
  {{ 0x1.0f5142p-4f, 0x1.0f50e0p-4f}, 0x1.c0f3f0p-1f}, //    49  
  {{ 0x1.126e6ep-4f, 0x1.126e0cp-4f}, 0x1.c103f0p-1f}, //    49  
  {{ 0x1.158beep-4f, 0x1.158b8ep-4f}, 0x1.c113f0p-1f}, //    48  
  {{ 0x1.18a960p-4f, 0x1.18a8fep-4f}, 0x1.c123eep-1f}, //    49  
  {{ 0x1.1bc724p-4f, 0x1.1bc6c2p-4f}, 0x1.c133ecp-1f}, //    49  
  {{ 0x1.1ee5a2p-4f, 0x1.1ee540p-4f}, 0x1.c143ecp-1f}, //    49  
  {{ 0x1.220472p-4f, 0x1.220410p-4f}, 0x1.c153ecp-1f}, //    49  
  {{ 0x1.252334p-4f, 0x1.2522d2p-4f}, 0x1.c163eap-1f}, //    49  
  {{ 0x1.28424ap-4f, 0x1.2841e8p-4f}, 0x1.c173e8p-1f}, //    49  
  {{ 0x1.2b6218p-4f, 0x1.2b61b6p-4f}, 0x1.c183e8p-1f}, //    49  
  {{ 0x1.2e823ap-4f, 0x1.2e81d8p-4f}, 0x1.c193e8p-1f}, //    49  
  {{ 0x1.31a24cp-4f, 0x1.31a1eap-4f}, 0x1.c1a3e6p-1f}, //    49  
  {{ 0x1.34c2b2p-4f, 0x1.34c250p-4f}, 0x1.c1b3e4p-1f}, //    49  
  {{ 0x1.37e3d2p-4f, 0x1.37e370p-4f}, 0x1.c1c3e4p-1f}, //    49  
  {{ 0x1.3b0544p-4f, 0x1.3b04e2p-4f}, 0x1.c1d3e4p-1f}, //    49  
  {{ 0x1.3e26a8p-4f, 0x1.3e2646p-4f}, 0x1.c1e3e2p-1f}, //    49  
  {{ 0x1.414860p-4f, 0x1.4147fep-4f}, 0x1.c1f3e0p-1f}, //    49  
  {{ 0x1.446ad0p-4f, 0x1.446a6ep-4f}, 0x1.c203e0p-1f}, //    49  
  {{ 0x1.478d94p-4f, 0x1.478d32p-4f}, 0x1.c213e0p-1f}, //    49  
  {{ 0x1.4ab04ap-4f, 0x1.4aafe6p-4f}, 0x1.c223dep-1f}, //    50  
  {{ 0x1.4dd352p-4f, 0x1.4dd2f0p-4f}, 0x1.c233dcp-1f}, //    49  
  {{ 0x1.50f714p-4f, 0x1.50f6b2p-4f}, 0x1.c243dcp-1f}, //    49  
  {{ 0x1.541b2ap-4f, 0x1.541ac8p-4f}, 0x1.c253dcp-1f}, //    49  
  {{ 0x1.573f30p-4f, 0x1.573ecep-4f}, 0x1.c263dap-1f}, //    49  
  {{ 0x1.5a638ap-4f, 0x1.5a6328p-4f}, 0x1.c273d8p-1f}, //    49  
  {{ 0x1.5d889ep-4f, 0x1.5d883cp-4f}, 0x1.c283d8p-1f}, //    49  
  {{ 0x1.60ae06p-4f, 0x1.60ada4p-4f}, 0x1.c293d8p-1f}, //    49  
  {{ 0x1.63d35ep-4f, 0x1.63d2fcp-4f}, 0x1.c2a3d6p-1f}, //    49  
  {{ 0x1.66f90ap-4f, 0x1.66f8a8p-4f}, 0x1.c2b3d4p-1f}, //    49  
  {{ 0x1.6a1f70p-4f, 0x1.6a1f0cp-4f}, 0x1.c2c3d4p-1f}, //    50  
  {{ 0x1.6d462ap-4f, 0x1.6d45c6p-4f}, 0x1.c2d3d4p-1f}, //    50  
  {{ 0x1.706cd2p-4f, 0x1.706c70p-4f}, 0x1.c2e3d2p-1f}, //    49  
  {{ 0x1.7393d0p-4f, 0x1.73936ep-4f}, 0x1.c2f3d0p-1f}, //    49  
  {{ 0x1.76bb88p-4f, 0x1.76bb24p-4f}, 0x1.c303d0p-1f}, //    50  
  {{ 0x1.79e394p-4f, 0x1.79e330p-4f}, 0x1.c313d0p-1f}, //    50  
  {{ 0x1.7d0b90p-4f, 0x1.7d0b2cp-4f}, 0x1.c323cep-1f}, //    50  
  {{ 0x1.8033e0p-4f, 0x1.80337cp-4f}, 0x1.c333ccp-1f}, //    50  
  {{ 0x1.835ce8p-4f, 0x1.835c86p-4f}, 0x1.c343ccp-1f}, //    49  
  {{ 0x1.868648p-4f, 0x1.8685e4p-4f}, 0x1.c353ccp-1f}, //    50  
  {{ 0x1.89af94p-4f, 0x1.89af32p-4f}, 0x1.c363cap-1f}, //    49  
  {{ 0x1.8cd936p-4f, 0x1.8cd8d4p-4f}, 0x1.c373c8p-1f}, //    49  
  {{ 0x1.900394p-4f, 0x1.900330p-4f}, 0x1.c383c8p-1f}, //    50  
  {{ 0x1.932e44p-4f, 0x1.932de0p-4f}, 0x1.c393c8p-1f}, //    50  
  {{ 0x1.9658e4p-4f, 0x1.965880p-4f}, 0x1.c3a3c6p-1f}, //    50  
  {{ 0x1.9983d8p-4f, 0x1.998374p-4f}, 0x1.c3b3c4p-1f}, //    50  
  {{ 0x1.9caf88p-4f, 0x1.9caf24p-4f}, 0x1.c3c3c4p-1f}, //    50  
  {{ 0x1.9fdb8ap-4f, 0x1.9fdb26p-4f}, 0x1.c3d3c4p-1f}, //    50  
  {{ 0x1.a3077ep-4f, 0x1.a3071ap-4f}, 0x1.c3e3c2p-1f}, //    50  
  {{ 0x1.a633c4p-4f, 0x1.a63360p-4f}, 0x1.c3f3c0p-1f}, //    50  
  {{ 0x1.a960c6p-4f, 0x1.a96062p-4f}, 0x1.c403c0p-1f}, //    50  
  {{ 0x1.ac8e1cp-4f, 0x1.ac8db8p-4f}, 0x1.c413c0p-1f}, //    50  
  {{ 0x1.afbb62p-4f, 0x1.afbafep-4f}, 0x1.c423bep-1f}, //    50  
  {{ 0x1.b2e8fcp-4f, 0x1.b2e898p-4f}, 0x1.c433bcp-1f}, //    50  
  {{ 0x1.b61750p-4f, 0x1.b616ecp-4f}, 0x1.c443bcp-1f}, //    50  
  {{ 0x1.b945fap-4f, 0x1.b94596p-4f}, 0x1.c453bcp-1f}, //    50  
  {{ 0x1.bc7492p-4f, 0x1.bc742ep-4f}, 0x1.c463bap-1f}, //    50  
  {{ 0x1.bfa37ep-4f, 0x1.bfa31ap-4f}, 0x1.c473b8p-1f}, //    50  
  {{ 0x1.c2d326p-4f, 0x1.c2d2c2p-4f}, 0x1.c483b8p-1f}, //    50  
  {{ 0x1.c60322p-4f, 0x1.c602bep-4f}, 0x1.c493b8p-1f}, //    50  
  {{ 0x1.c9330ep-4f, 0x1.c932aap-4f}, 0x1.c4a3b6p-1f}, //    50  
  {{ 0x1.cc634ep-4f, 0x1.cc62eap-4f}, 0x1.c4b3b4p-1f}, //    50  
  {{ 0x1.cf944ap-4f, 0x1.cf93e4p-4f}, 0x1.c4c3b4p-1f}, //    51  
  {{ 0x1.d2c59ap-4f, 0x1.d2c534p-4f}, 0x1.c4d3b4p-1f}, //    51  
  {{ 0x1.d5f6d8p-4f, 0x1.d5f674p-4f}, 0x1.c4e3b2p-1f}, //    50  
  {{ 0x1.d9286cp-4f, 0x1.d92806p-4f}, 0x1.c4f3b0p-1f}, //    51  
  {{ 0x1.dc5abap-4f, 0x1.dc5a56p-4f}, 0x1.c503b0p-1f}, //    50  
  {{ 0x1.df8d5ep-4f, 0x1.df8cfap-4f}, 0x1.c513b0p-1f}, //    50  
  {{ 0x1.e2bff0p-4f, 0x1.e2bf8cp-4f}, 0x1.c523aep-1f}, //    50  
  {{ 0x1.e5f2d6p-4f, 0x1.e5f272p-4f}, 0x1.c533acp-1f}, //    50  
  {{ 0x1.e92678p-4f, 0x1.e92614p-4f}, 0x1.c543acp-1f}, //    50  
  {{ 0x1.ec5a70p-4f, 0x1.ec5a0cp-4f}, 0x1.c553acp-1f}, //    50  
  {{ 0x1.ef8e56p-4f, 0x1.ef8df2p-4f}, 0x1.c563aap-1f}, //    50  
  {{ 0x1.f2c290p-4f, 0x1.f2c22cp-4f}, 0x1.c573a8p-1f}, //    50  
  {{ 0x1.f5f788p-4f, 0x1.f5f722p-4f}, 0x1.c583a8p-1f}, //    51  
  {{ 0x1.f92cd2p-4f, 0x1.f92c6ep-4f}, 0x1.c593a8p-1f}, //    50  
  {{ 0x1.fc620cp-4f, 0x1.fc61a8p-4f}, 0x1.c5a3a6p-1f}, //    50  
  {{ 0x1.ff979ap-4f, 0x1.ff9736p-4f}, 0x1.c5b3a4p-1f}, //    50  
  {{ 0x1.0166f2p-3f, 0x1.0166c0p-3f}, 0x1.c5c3a4p-1f}, //    25  
  {{ 0x1.030242p-3f, 0x1.030210p-3f}, 0x1.c5d3a4p-1f}, //    25  
  {{ 0x1.049d88p-3f, 0x1.049d58p-3f}, 0x1.c5e3a2p-1f}, //    24  
  {{ 0x1.0638fap-3f, 0x1.0638c8p-3f}, 0x1.c5f3a0p-1f}, //    25  
  {{ 0x1.07d4cap-3f, 0x1.07d498p-3f}, 0x1.c603a0p-1f}, //    25  
  {{ 0x1.0970c4p-3f, 0x1.097092p-3f}, 0x1.c613a0p-1f}, //    25  
  {{ 0x1.0b0cb4p-3f, 0x1.0b0c82p-3f}, 0x1.c6239ep-1f}, //    25  
  {{ 0x1.0ca8d0p-3f, 0x1.0ca89ep-3f}, 0x1.c6339cp-1f}, //    25  
  {{ 0x1.0e454ap-3f, 0x1.0e4518p-3f}, 0x1.c6439cp-1f}, //    25  
  {{ 0x1.0fe1eep-3f, 0x1.0fe1bcp-3f}, 0x1.c6539cp-1f}, //    25  
  {{ 0x1.117e88p-3f, 0x1.117e58p-3f}, 0x1.c6639ap-1f}, //    24  
  {{ 0x1.131b4ep-3f, 0x1.131b1ep-3f}, 0x1.c67398p-1f}, //    24  
  {{ 0x1.14b872p-3f, 0x1.14b840p-3f}, 0x1.c68398p-1f}, //    25  
  {{ 0x1.1655c2p-3f, 0x1.165590p-3f}, 0x1.c69398p-1f}, //    25  
  {{ 0x1.17f306p-3f, 0x1.17f2d4p-3f}, 0x1.c6a396p-1f}, //    25  
  {{ 0x1.199076p-3f, 0x1.199046p-3f}, 0x1.c6b394p-1f}, //    24  
  {{ 0x1.1b2e46p-3f, 0x1.1b2e14p-3f}, 0x1.c6c394p-1f}, //    25  
  {{ 0x1.1ccc3ep-3f, 0x1.1ccc0cp-3f}, 0x1.c6d394p-1f}, //    25  
  {{ 0x1.1e6a2ep-3f, 0x1.1e69fcp-3f}, 0x1.c6e392p-1f}, //    25  
  {{ 0x1.200848p-3f, 0x1.200816p-3f}, 0x1.c6f390p-1f}, //    25  
  {{ 0x1.21a6c2p-3f, 0x1.21a690p-3f}, 0x1.c70390p-1f}, //    25  
  {{ 0x1.234566p-3f, 0x1.234534p-3f}, 0x1.c71390p-1f}, //    25  
  {{ 0x1.24e400p-3f, 0x1.24e3cep-3f}, 0x1.c7238ep-1f}, //    25  
  {{ 0x1.2682c4p-3f, 0x1.268292p-3f}, 0x1.c7338cp-1f}, //    25  
  {{ 0x1.2821e8p-3f, 0x1.2821b6p-3f}, 0x1.c7438cp-1f}, //    25  
  {{ 0x1.29c136p-3f, 0x1.29c104p-3f}, 0x1.c7538cp-1f}, //    25  
  {{ 0x1.2b607cp-3f, 0x1.2b604ap-3f}, 0x1.c7638ap-1f}, //    25  
  {{ 0x1.2cffeap-3f, 0x1.2cffb8p-3f}, 0x1.c77388p-1f}, //    25  
  {{ 0x1.2e9fbap-3f, 0x1.2e9f88p-3f}, 0x1.c78388p-1f}, //    25  
  {{ 0x1.303fb2p-3f, 0x1.303f80p-3f}, 0x1.c79388p-1f}, //    25  
  {{ 0x1.31dfa2p-3f, 0x1.31df70p-3f}, 0x1.c7a386p-1f}, //    25  
  {{ 0x1.337fbcp-3f, 0x1.337f8ap-3f}, 0x1.c7b384p-1f}, //    25  
  {{ 0x1.352036p-3f, 0x1.352004p-3f}, 0x1.c7c384p-1f}, //    25  
  {{ 0x1.36c0dap-3f, 0x1.36c0a6p-3f}, 0x1.c7d384p-1f}, //    26  
  {{ 0x1.386174p-3f, 0x1.386142p-3f}, 0x1.c7e382p-1f}, //    25  
  {{ 0x1.3a0238p-3f, 0x1.3a0206p-3f}, 0x1.c7f380p-1f}, //    25  
  {{ 0x1.3ba35cp-3f, 0x1.3ba32ap-3f}, 0x1.c80380p-1f}, //    25  
  {{ 0x1.3d44acp-3f, 0x1.3d447ap-3f}, 0x1.c81380p-1f}, //    25  
  {{ 0x1.3ee5f0p-3f, 0x1.3ee5bep-3f}, 0x1.c8237ep-1f}, //    25  
  {{ 0x1.408760p-3f, 0x1.40872ep-3f}, 0x1.c8337cp-1f}, //    25  
  {{ 0x1.422930p-3f, 0x1.4228fep-3f}, 0x1.c8437cp-1f}, //    25  
  {{ 0x1.43cb2ap-3f, 0x1.43caf8p-3f}, 0x1.c8537cp-1f}, //    25  
  {{ 0x1.456d1ap-3f, 0x1.456ce8p-3f}, 0x1.c8637ap-1f}, //    25  
  {{ 0x1.470f34p-3f, 0x1.470f02p-3f}, 0x1.c87378p-1f}, //    25  
  {{ 0x1.48b1aep-3f, 0x1.48b17cp-3f}, 0x1.c88378p-1f}, //    25  
  {{ 0x1.4a5454p-3f, 0x1.4a5422p-3f}, 0x1.c89378p-1f}, //    25  
  {{ 0x1.4bf6eep-3f, 0x1.4bf6bcp-3f}, 0x1.c8a376p-1f}, //    25  
  {{ 0x1.4d99b6p-3f, 0x1.4d9982p-3f}, 0x1.c8b374p-1f}, //    26  
  {{ 0x1.4f3cdap-3f, 0x1.4f3ca8p-3f}, 0x1.c8c374p-1f}, //    25  
  {{ 0x1.50e02ap-3f, 0x1.50dff8p-3f}, 0x1.c8d374p-1f}, //    25  
  {{ 0x1.528370p-3f, 0x1.52833ep-3f}, 0x1.c8e372p-1f}, //    25  
  {{ 0x1.5426e2p-3f, 0x1.5426b0p-3f}, 0x1.c8f370p-1f}, //    25  
  {{ 0x1.55cab2p-3f, 0x1.55ca80p-3f}, 0x1.c90370p-1f}, //    25  
  {{ 0x1.576eaep-3f, 0x1.576e7cp-3f}, 0x1.c91370p-1f}, //    25  
  {{ 0x1.5912a0p-3f, 0x1.59126ep-3f}, 0x1.c9236ep-1f}, //    25  
  {{ 0x1.5ab6bcp-3f, 0x1.5ab68ap-3f}, 0x1.c9336cp-1f}, //    25  
  {{ 0x1.5c5b38p-3f, 0x1.5c5b06p-3f}, 0x1.c9436cp-1f}, //    25  
  {{ 0x1.5dffdep-3f, 0x1.5dffacp-3f}, 0x1.c9536cp-1f}, //    25  
  {{ 0x1.5fa47cp-3f, 0x1.5fa44ap-3f}, 0x1.c9636ap-1f}, //    25  
  {{ 0x1.614944p-3f, 0x1.614912p-3f}, 0x1.c97368p-1f}, //    25  
  {{ 0x1.62ee6cp-3f, 0x1.62ee38p-3f}, 0x1.c98368p-1f}, //    26  
  {{ 0x1.6493bep-3f, 0x1.64938ap-3f}, 0x1.c99368p-1f}, //    26  
  {{ 0x1.663906p-3f, 0x1.6638d4p-3f}, 0x1.c9a366p-1f}, //    25  
  {{ 0x1.67de7ap-3f, 0x1.67de46p-3f}, 0x1.c9b364p-1f}, //    26  
  {{ 0x1.69844cp-3f, 0x1.69841ap-3f}, 0x1.c9c364p-1f}, //    25  
  {{ 0x1.6b2a4ap-3f, 0x1.6b2a18p-3f}, 0x1.c9d364p-1f}, //    25  
  {{ 0x1.6cd03ep-3f, 0x1.6cd00cp-3f}, 0x1.c9e362p-1f}, //    25  
  {{ 0x1.6e765ep-3f, 0x1.6e762ap-3f}, 0x1.c9f360p-1f}, //    26  
  {{ 0x1.701cdcp-3f, 0x1.701ca8p-3f}, 0x1.ca0360p-1f}, //    26  
  {{ 0x1.71c386p-3f, 0x1.71c352p-3f}, 0x1.ca1360p-1f}, //    26  
  {{ 0x1.736a24p-3f, 0x1.7369f2p-3f}, 0x1.ca235ep-1f}, //    25  
  {{ 0x1.7510f0p-3f, 0x1.7510bcp-3f}, 0x1.ca335cp-1f}, //    26  
  {{ 0x1.76b81ap-3f, 0x1.76b7e6p-3f}, 0x1.ca435cp-1f}, //    26  
  {{ 0x1.785f70p-3f, 0x1.785f3cp-3f}, 0x1.ca535cp-1f}, //    26  
  {{ 0x1.7a06bap-3f, 0x1.7a0688p-3f}, 0x1.ca635ap-1f}, //    25  
  {{ 0x1.7bae30p-3f, 0x1.7badfep-3f}, 0x1.ca7358p-1f}, //    25  
  {{ 0x1.7d5606p-3f, 0x1.7d55d4p-3f}, 0x1.ca8358p-1f}, //    25  
  {{ 0x1.7efe08p-3f, 0x1.7efdd4p-3f}, 0x1.ca9358p-1f}, //    26  
  {{ 0x1.80a600p-3f, 0x1.80a5ccp-3f}, 0x1.caa356p-1f}, //    26  
  {{ 0x1.824e22p-3f, 0x1.824deep-3f}, 0x1.cab354p-1f}, //    26  
  {{ 0x1.83f6a4p-3f, 0x1.83f670p-3f}, 0x1.cac354p-1f}, //    26  
  {{ 0x1.859f50p-3f, 0x1.859f1ep-3f}, 0x1.cad354p-1f}, //    25  
  {{ 0x1.8747f4p-3f, 0x1.8747c0p-3f}, 0x1.cae352p-1f}, //    26  
  {{ 0x1.88f0c2p-3f, 0x1.88f08ep-3f}, 0x1.caf350p-1f}, //    26  
  {{ 0x1.8a99f0p-3f, 0x1.8a99bcp-3f}, 0x1.cb0350p-1f}, //    26  
  {{ 0x1.8c4348p-3f, 0x1.8c4316p-3f}, 0x1.cb1350p-1f}, //    25  
  {{ 0x1.8dec98p-3f, 0x1.8dec64p-3f}, 0x1.cb234ep-1f}, //    26  
  {{ 0x1.8f9612p-3f, 0x1.8f95dep-3f}, 0x1.cb334cp-1f}, //    26  
  {{ 0x1.913fecp-3f, 0x1.913fb8p-3f}, 0x1.cb434cp-1f}, //    26  
  {{ 0x1.92e9f2p-3f, 0x1.92e9bep-3f}, 0x1.cb534cp-1f}, //    26  
  {{ 0x1.9493ecp-3f, 0x1.9493bap-3f}, 0x1.cb634ap-1f}, //    25  
  {{ 0x1.963e12p-3f, 0x1.963de0p-3f}, 0x1.cb7348p-1f}, //    25  
  {{ 0x1.97e89ap-3f, 0x1.97e866p-3f}, 0x1.cb8348p-1f}, //    26  
  {{ 0x1.99934ap-3f, 0x1.999318p-3f}, 0x1.cb9348p-1f}, //    25  
  {{ 0x1.9b3df2p-3f, 0x1.9b3dbep-3f}, 0x1.cba346p-1f}, //    26  
  {{ 0x1.9ce8c4p-3f, 0x1.9ce892p-3f}, 0x1.cbb344p-1f}, //    25  
  {{ 0x1.9e93f8p-3f, 0x1.9e93c4p-3f}, 0x1.cbc344p-1f}, //    26  
  {{ 0x1.a03f56p-3f, 0x1.a03f22p-3f}, 0x1.cbd344p-1f}, //    26  
  {{ 0x1.a1eaa8p-3f, 0x1.a1ea76p-3f}, 0x1.cbe342p-1f}, //    25  
  {{ 0x1.a39628p-3f, 0x1.a395f4p-3f}, 0x1.cbf340p-1f}, //    26  
  {{ 0x1.a54206p-3f, 0x1.a541d4p-3f}, 0x1.cc0340p-1f}, //    25  
  {{ 0x1.a6ee10p-3f, 0x1.a6eddep-3f}, 0x1.cc1340p-1f}, //    25  
  {{ 0x1.a89a10p-3f, 0x1.a899dep-3f}, 0x1.cc233ep-1f}, //    25  
  {{ 0x1.aa463cp-3f, 0x1.aa4608p-3f}, 0x1.cc333cp-1f}, //    26  
  {{ 0x1.abf2c8p-3f, 0x1.abf294p-3f}, 0x1.cc433cp-1f}, //    26  
  {{ 0x1.ad9f7ep-3f, 0x1.ad9f4ap-3f}, 0x1.cc533cp-1f}, //    26  
  {{ 0x1.af4c2ap-3f, 0x1.af4bf8p-3f}, 0x1.cc633ap-1f}, //    25  
  {{ 0x1.b0f902p-3f, 0x1.b0f8cep-3f}, 0x1.cc7338p-1f}, //    26  
  {{ 0x1.b2a63ap-3f, 0x1.b2a606p-3f}, 0x1.cc8338p-1f}, //    26  
  {{ 0x1.b4539ep-3f, 0x1.b4536ap-3f}, 0x1.cc9338p-1f}, //    26  
  {{ 0x1.b600f8p-3f, 0x1.b600c4p-3f}, 0x1.cca336p-1f}, //    26  
  {{ 0x1.b7ae7cp-3f, 0x1.b7ae48p-3f}, 0x1.ccb334p-1f}, //    26  
  {{ 0x1.b95c60p-3f, 0x1.b95c2cp-3f}, 0x1.ccc334p-1f}, //    26  
  {{ 0x1.bb0a70p-3f, 0x1.bb0a3cp-3f}, 0x1.ccd334p-1f}, //    26  
  {{ 0x1.bcb876p-3f, 0x1.bcb842p-3f}, 0x1.cce332p-1f}, //    26  
  {{ 0x1.be66a6p-3f, 0x1.be6672p-3f}, 0x1.ccf330p-1f}, //    26  
  {{ 0x1.c01538p-3f, 0x1.c01504p-3f}, 0x1.cd0330p-1f}, //    26  
  {{ 0x1.c1c3f6p-3f, 0x1.c1c3c2p-3f}, 0x1.cd1330p-1f}, //    26  
  {{ 0x1.c372a8p-3f, 0x1.c37274p-3f}, 0x1.cd232ep-1f}, //    26  
  {{ 0x1.c52186p-3f, 0x1.c52152p-3f}, 0x1.cd332cp-1f}, //    26  
  {{ 0x1.c6d0c4p-3f, 0x1.c6d090p-3f}, 0x1.cd432cp-1f}, //    26  
  {{ 0x1.c8802ep-3f, 0x1.c87ffap-3f}, 0x1.cd532cp-1f}, //    26  
  {{ 0x1.ca2f8cp-3f, 0x1.ca2f58p-3f}, 0x1.cd632ap-1f}, //    26  
  {{ 0x1.cbdf18p-3f, 0x1.cbdee4p-3f}, 0x1.cd7328p-1f}, //    26  
  {{ 0x1.cd8f02p-3f, 0x1.cd8ecep-3f}, 0x1.cd8328p-1f}, //    26  
  {{ 0x1.cf3f1ap-3f, 0x1.cf3ee6p-3f}, 0x1.cd9328p-1f}, //    26  
  {{ 0x1.d0ef26p-3f, 0x1.d0eef2p-3f}, 0x1.cda326p-1f}, //    26  
  {{ 0x1.d29f5ep-3f, 0x1.d29f2ap-3f}, 0x1.cdb324p-1f}, //    26  
  {{ 0x1.d44ff6p-3f, 0x1.d44fc2p-3f}, 0x1.cdc324p-1f}, //    26  
  {{ 0x1.d600bap-3f, 0x1.d60086p-3f}, 0x1.cdd324p-1f}, //    26  
  {{ 0x1.d7b172p-3f, 0x1.d7b13ep-3f}, 0x1.cde322p-1f}, //    26  
  {{ 0x1.d96258p-3f, 0x1.d96224p-3f}, 0x1.cdf320p-1f}, //    26  
  {{ 0x1.db139ep-3f, 0x1.db1368p-3f}, 0x1.ce0320p-1f}, //    27  
  {{ 0x1.dcc50ep-3f, 0x1.dcc4dap-3f}, 0x1.ce1320p-1f}, //    26  
  {{ 0x1.de7674p-3f, 0x1.de7640p-3f}, 0x1.ce231ep-1f}, //    26  
  {{ 0x1.e02806p-3f, 0x1.e027d2p-3f}, 0x1.ce331cp-1f}, //    26  
  {{ 0x1.e1d9fap-3f, 0x1.e1d9c4p-3f}, 0x1.ce431cp-1f}, //    27  
  {{ 0x1.e38c18p-3f, 0x1.e38be4p-3f}, 0x1.ce531cp-1f}, //    26  
  {{ 0x1.e53e2ap-3f, 0x1.e53df6p-3f}, 0x1.ce631ap-1f}, //    26  
  {{ 0x1.e6f06ap-3f, 0x1.e6f036p-3f}, 0x1.ce7318p-1f}, //    26  
  {{ 0x1.e8a30ap-3f, 0x1.e8a2d6p-3f}, 0x1.ce8318p-1f}, //    26  
  {{ 0x1.ea55d6p-3f, 0x1.ea55a2p-3f}, 0x1.ce9318p-1f}, //    26  
  {{ 0x1.ec0896p-3f, 0x1.ec0862p-3f}, 0x1.cea316p-1f}, //    26  
  {{ 0x1.edbb82p-3f, 0x1.edbb4ep-3f}, 0x1.ceb314p-1f}, //    26  
  {{ 0x1.ef6ed0p-3f, 0x1.ef6e9cp-3f}, 0x1.cec314p-1f}, //    26  
  {{ 0x1.f1224ap-3f, 0x1.f12216p-3f}, 0x1.ced314p-1f}, //    26  
  {{ 0x1.f2d5b8p-3f, 0x1.f2d584p-3f}, 0x1.cee312p-1f}, //    26  
  {{ 0x1.f48952p-3f, 0x1.f4891cp-3f}, 0x1.cef310p-1f}, //    27  
  {{ 0x1.f63d4cp-3f, 0x1.f63d18p-3f}, 0x1.cf0310p-1f}, //    26  
  {{ 0x1.f7f174p-3f, 0x1.f7f13ep-3f}, 0x1.cf1310p-1f}, //    27  
  {{ 0x1.f9a590p-3f, 0x1.f9a55ap-3f}, 0x1.cf230ep-1f}, //    27  
  {{ 0x1.fb59d6p-3f, 0x1.fb59a2p-3f}, 0x1.cf330cp-1f}, //    26  
  {{ 0x1.fd0e80p-3f, 0x1.fd0e4ap-3f}, 0x1.cf430cp-1f}, //    27  
  {{ 0x1.fec354p-3f, 0x1.fec31ep-3f}, 0x1.cf530cp-1f}, //    27  
  {{ 0x1.003c0ep-2f, 0x1.003bf4p-2f}, 0x1.cf630ap-1f}, //    13  
  {{ 0x1.011688p-2f, 0x1.01166ep-2f}, 0x1.cf7308p-1f}, //    13  
  {{ 0x1.01f134p-2f, 0x1.01f11ap-2f}, 0x1.cf8308p-1f}, //    13  
  {{ 0x1.02cbf4p-2f, 0x1.02cbdcp-2f}, 0x1.cf9308p-1f}, //    12  
  {{ 0x1.03a6b0p-2f, 0x1.03a696p-2f}, 0x1.cfa306p-1f}, //    13  
  {{ 0x1.048182p-2f, 0x1.048168p-2f}, 0x1.cfb304p-1f}, //    13  
  {{ 0x1.055c84p-2f, 0x1.055c6ap-2f}, 0x1.cfc304p-1f}, //    13  
  {{ 0x1.06379cp-2f, 0x1.063782p-2f}, 0x1.cfd304p-1f}, //    13  
  {{ 0x1.0712aep-2f, 0x1.071294p-2f}, 0x1.cfe302p-1f}, //    13  
  {{ 0x1.07edd6p-2f, 0x1.07edbcp-2f}, 0x1.cff300p-1f}, //    13  
  {{ 0x1.08c930p-2f, 0x1.08c916p-2f}, 0x1.d00300p-1f}, //    13  
  {{ 0x1.09a49ep-2f, 0x1.09a484p-2f}, 0x1.d01300p-1f}, //    13  
  {{ 0x1.0a8008p-2f, 0x1.0a7feep-2f}, 0x1.d022fep-1f}, //    13  
  {{ 0x1.0b5b86p-2f, 0x1.0b5b6cp-2f}, 0x1.d032fcp-1f}, //    13  
  {{ 0x1.0c3736p-2f, 0x1.0c371ep-2f}, 0x1.d042fcp-1f}, //    12  
  {{ 0x1.0d12fcp-2f, 0x1.0d12e4p-2f}, 0x1.d052fcp-1f}, //    12  
  {{ 0x1.0deebcp-2f, 0x1.0deea4p-2f}, 0x1.d062fap-1f}, //    12  
  {{ 0x1.0eca92p-2f, 0x1.0eca7ap-2f}, 0x1.d072f8p-1f}, //    12  
  {{ 0x1.0fa69ap-2f, 0x1.0fa680p-2f}, 0x1.d082f8p-1f}, //    13  
  {{ 0x1.1082b8p-2f, 0x1.10829ep-2f}, 0x1.d092f8p-1f}, //    13  
  {{ 0x1.115ecep-2f, 0x1.115eb4p-2f}, 0x1.d0a2f6p-1f}, //    13  
  {{ 0x1.123afcp-2f, 0x1.123ae2p-2f}, 0x1.d0b2f4p-1f}, //    13  
  {{ 0x1.13175ap-2f, 0x1.131740p-2f}, 0x1.d0c2f4p-1f}, //    13  
  {{ 0x1.13f3cep-2f, 0x1.13f3b4p-2f}, 0x1.d0d2f4p-1f}, //    13  
  {{ 0x1.14d03cp-2f, 0x1.14d022p-2f}, 0x1.d0e2f2p-1f}, //    13  
  {{ 0x1.15acc0p-2f, 0x1.15aca6p-2f}, 0x1.d0f2f0p-1f}, //    13  
  {{ 0x1.168976p-2f, 0x1.16895cp-2f}, 0x1.d102f0p-1f}, //    13  
  {{ 0x1.176642p-2f, 0x1.176628p-2f}, 0x1.d112f0p-1f}, //    13  
  {{ 0x1.184306p-2f, 0x1.1842ecp-2f}, 0x1.d122eep-1f}, //    13  
  {{ 0x1.191fe2p-2f, 0x1.191fc8p-2f}, 0x1.d132ecp-1f}, //    13  
  {{ 0x1.19fceep-2f, 0x1.19fcd4p-2f}, 0x1.d142ecp-1f}, //    13  
  {{ 0x1.1ada10p-2f, 0x1.1ad9f8p-2f}, 0x1.d152ecp-1f}, //    12  
  {{ 0x1.1bb72ep-2f, 0x1.1bb714p-2f}, 0x1.d162eap-1f}, //    13  
  {{ 0x1.1c9460p-2f, 0x1.1c9446p-2f}, 0x1.d172e8p-1f}, //    13  
  {{ 0x1.1d71c4p-2f, 0x1.1d71aap-2f}, 0x1.d182e8p-1f}, //    13  
  {{ 0x1.1e4f3ep-2f, 0x1.1e4f24p-2f}, 0x1.d192e8p-1f}, //    13  
  {{ 0x1.1f2cb2p-2f, 0x1.1f2c98p-2f}, 0x1.d1a2e6p-1f}, //    13  
  {{ 0x1.200a3cp-2f, 0x1.200a22p-2f}, 0x1.d1b2e4p-1f}, //    13  
  {{ 0x1.20e7f6p-2f, 0x1.20e7dcp-2f}, 0x1.d1c2e4p-1f}, //    13  
  {{ 0x1.21c5c8p-2f, 0x1.21c5aep-2f}, 0x1.d1d2e4p-1f}, //    13  
  {{ 0x1.22a392p-2f, 0x1.22a378p-2f}, 0x1.d1e2e2p-1f}, //    13  
  {{ 0x1.238174p-2f, 0x1.23815ap-2f}, 0x1.d1f2e0p-1f}, //    13  
  {{ 0x1.245f86p-2f, 0x1.245f6cp-2f}, 0x1.d202e0p-1f}, //    13  
  {{ 0x1.253daep-2f, 0x1.253d94p-2f}, 0x1.d212e0p-1f}, //    13  
  {{ 0x1.261bd0p-2f, 0x1.261bb6p-2f}, 0x1.d222dep-1f}, //    13  
  {{ 0x1.26fa08p-2f, 0x1.26f9f0p-2f}, 0x1.d232dcp-1f}, //    12  
  {{ 0x1.27d872p-2f, 0x1.27d858p-2f}, 0x1.d242dcp-1f}, //    13  
  {{ 0x1.28b6f2p-2f, 0x1.28b6d8p-2f}, 0x1.d252dcp-1f}, //    13  
  {{ 0x1.29956cp-2f, 0x1.299552p-2f}, 0x1.d262dap-1f}, //    13  
  {{ 0x1.2a73fcp-2f, 0x1.2a73e2p-2f}, 0x1.d272d8p-1f}, //    13  
  {{ 0x1.2b52bep-2f, 0x1.2b52a4p-2f}, 0x1.d282d8p-1f}, //    13  
  {{ 0x1.2c3194p-2f, 0x1.2c317ap-2f}, 0x1.d292d8p-1f}, //    13  
  {{ 0x1.2d1066p-2f, 0x1.2d104cp-2f}, 0x1.d2a2d6p-1f}, //    13  
  {{ 0x1.2def4cp-2f, 0x1.2def32p-2f}, 0x1.d2b2d4p-1f}, //    13  
  {{ 0x1.2ece66p-2f, 0x1.2ece4cp-2f}, 0x1.d2c2d4p-1f}, //    13  
  {{ 0x1.2fad94p-2f, 0x1.2fad7ap-2f}, 0x1.d2d2d4p-1f}, //    13  
  {{ 0x1.308cbcp-2f, 0x1.308ca2p-2f}, 0x1.d2e2d2p-1f}, //    13  
  {{ 0x1.316bfcp-2f, 0x1.316be2p-2f}, 0x1.d2f2d0p-1f}, //    13  
  {{ 0x1.324b6cp-2f, 0x1.324b52p-2f}, 0x1.d302d0p-1f}, //    13  
  {{ 0x1.332af2p-2f, 0x1.332ad8p-2f}, 0x1.d312d0p-1f}, //    13  
  {{ 0x1.340a72p-2f, 0x1.340a58p-2f}, 0x1.d322cep-1f}, //    13  
  {{ 0x1.34ea08p-2f, 0x1.34e9eep-2f}, 0x1.d332ccp-1f}, //    13  
  {{ 0x1.35c9d0p-2f, 0x1.35c9b6p-2f}, 0x1.d342ccp-1f}, //    13  
  {{ 0x1.36a9aep-2f, 0x1.36a994p-2f}, 0x1.d352ccp-1f}, //    13  
  {{ 0x1.378984p-2f, 0x1.37896ap-2f}, 0x1.d362cap-1f}, //    13  
  {{ 0x1.386972p-2f, 0x1.386958p-2f}, 0x1.d372c8p-1f}, //    13  
  {{ 0x1.394992p-2f, 0x1.394978p-2f}, 0x1.d382c8p-1f}, //    13  
  {{ 0x1.3a29c8p-2f, 0x1.3a29aep-2f}, 0x1.d392c8p-1f}, //    13  
  {{ 0x1.3b09f6p-2f, 0x1.3b09dcp-2f}, 0x1.d3a2c6p-1f}, //    13  
  {{ 0x1.3bea3cp-2f, 0x1.3bea22p-2f}, 0x1.d3b2c4p-1f}, //    13  
  {{ 0x1.3ccab2p-2f, 0x1.3cca98p-2f}, 0x1.d3c2c4p-1f}, //    13  
  {{ 0x1.3dab40p-2f, 0x1.3dab26p-2f}, 0x1.d3d2c4p-1f}, //    13  
  {{ 0x1.3e8bc6p-2f, 0x1.3e8bacp-2f}, 0x1.d3e2c2p-1f}, //    13  
  {{ 0x1.3f6c64p-2f, 0x1.3f6c4ap-2f}, 0x1.d3f2c0p-1f}, //    13  
  {{ 0x1.404d32p-2f, 0x1.404d18p-2f}, 0x1.d402c0p-1f}, //    13  
  {{ 0x1.412e16p-2f, 0x1.412dfcp-2f}, 0x1.d412c0p-1f}, //    13  
  {{ 0x1.420ef6p-2f, 0x1.420edcp-2f}, 0x1.d422bep-1f}, //    13  
  {{ 0x1.42efeap-2f, 0x1.42efd0p-2f}, 0x1.d432bcp-1f}, //    13  
  {{ 0x1.43d110p-2f, 0x1.43d0f6p-2f}, 0x1.d442bcp-1f}, //    13  
  {{ 0x1.44b24ep-2f, 0x1.44b232p-2f}, 0x1.d452bcp-1f}, //    14  
  {{ 0x1.459384p-2f, 0x1.45936ap-2f}, 0x1.d462bap-1f}, //    13  
  {{ 0x1.4674d0p-2f, 0x1.4674b6p-2f}, 0x1.d472b8p-1f}, //    13  
  {{ 0x1.47564ep-2f, 0x1.475634p-2f}, 0x1.d482b8p-1f}, //    13  
  {{ 0x1.4837e2p-2f, 0x1.4837c8p-2f}, 0x1.d492b8p-1f}, //    13  
  {{ 0x1.491970p-2f, 0x1.491956p-2f}, 0x1.d4a2b6p-1f}, //    13  
  {{ 0x1.49fb14p-2f, 0x1.49fafap-2f}, 0x1.d4b2b4p-1f}, //    13  
  {{ 0x1.4adceap-2f, 0x1.4adcd0p-2f}, 0x1.d4c2b4p-1f}, //    13  
  {{ 0x1.4bbed6p-2f, 0x1.4bbebcp-2f}, 0x1.d4d2b4p-1f}, //    13  
  {{ 0x1.4ca0bcp-2f, 0x1.4ca0a2p-2f}, 0x1.d4e2b2p-1f}, //    13  
  {{ 0x1.4d82b8p-2f, 0x1.4d829ep-2f}, 0x1.d4f2b0p-1f}, //    13  
  {{ 0x1.4e64e6p-2f, 0x1.4e64ccp-2f}, 0x1.d502b0p-1f}, //    13  
  {{ 0x1.4f472ap-2f, 0x1.4f4710p-2f}, 0x1.d512b0p-1f}, //    13  
  {{ 0x1.502968p-2f, 0x1.50294ep-2f}, 0x1.d522aep-1f}, //    13  
  {{ 0x1.510bbcp-2f, 0x1.510ba2p-2f}, 0x1.d532acp-1f}, //    13  
  {{ 0x1.51ee42p-2f, 0x1.51ee28p-2f}, 0x1.d542acp-1f}, //    13  
  {{ 0x1.52d0dep-2f, 0x1.52d0c4p-2f}, 0x1.d552acp-1f}, //    13  
  {{ 0x1.53b374p-2f, 0x1.53b35ap-2f}, 0x1.d562aap-1f}, //    13  
  {{ 0x1.549620p-2f, 0x1.549604p-2f}, 0x1.d572a8p-1f}, //    14  
  {{ 0x1.5578fep-2f, 0x1.5578e2p-2f}, 0x1.d582a8p-1f}, //    14  
  {{ 0x1.565bf2p-2f, 0x1.565bd6p-2f}, 0x1.d592a8p-1f}, //    14  
  {{ 0x1.573edep-2f, 0x1.573ec4p-2f}, 0x1.d5a2a6p-1f}, //    13  
  {{ 0x1.5821e2p-2f, 0x1.5821c8p-2f}, 0x1.d5b2a4p-1f}, //    13  
  {{ 0x1.590518p-2f, 0x1.5904fep-2f}, 0x1.d5c2a4p-1f}, //    13  
  {{ 0x1.59e864p-2f, 0x1.59e84ap-2f}, 0x1.d5d2a4p-1f}, //    13  
  {{ 0x1.5acbaap-2f, 0x1.5acb90p-2f}, 0x1.d5e2a2p-1f}, //    13  
  {{ 0x1.5baf06p-2f, 0x1.5baeecp-2f}, 0x1.d5f2a0p-1f}, //    13  
  {{ 0x1.5c9294p-2f, 0x1.5c927ap-2f}, 0x1.d602a0p-1f}, //    13  
  {{ 0x1.5d7638p-2f, 0x1.5d761ep-2f}, 0x1.d612a0p-1f}, //    13  
  {{ 0x1.5e59d6p-2f, 0x1.5e59bcp-2f}, 0x1.d6229ep-1f}, //    13  
  {{ 0x1.5f3d8ap-2f, 0x1.5f3d70p-2f}, 0x1.d6329cp-1f}, //    13  
  {{ 0x1.602170p-2f, 0x1.602156p-2f}, 0x1.d6429cp-1f}, //    13  
  {{ 0x1.61056cp-2f, 0x1.610552p-2f}, 0x1.d6529cp-1f}, //    13  
  {{ 0x1.61e962p-2f, 0x1.61e948p-2f}, 0x1.d6629ap-1f}, //    13  
  {{ 0x1.62cd6ep-2f, 0x1.62cd54p-2f}, 0x1.d67298p-1f}, //    13  
  {{ 0x1.63b1acp-2f, 0x1.63b192p-2f}, 0x1.d68298p-1f}, //    13  
  {{ 0x1.649600p-2f, 0x1.6495e6p-2f}, 0x1.d69298p-1f}, //    13  
  {{ 0x1.657a4ep-2f, 0x1.657a34p-2f}, 0x1.d6a296p-1f}, //    13  
  {{ 0x1.665eb2p-2f, 0x1.665e98p-2f}, 0x1.d6b294p-1f}, //    13  
  {{ 0x1.67434ap-2f, 0x1.67432ep-2f}, 0x1.d6c294p-1f}, //    14  
  {{ 0x1.6827f6p-2f, 0x1.6827dcp-2f}, 0x1.d6d294p-1f}, //    13  
  {{ 0x1.690c9cp-2f, 0x1.690c82p-2f}, 0x1.d6e292p-1f}, //    13  
  {{ 0x1.69f158p-2f, 0x1.69f13ep-2f}, 0x1.d6f290p-1f}, //    13  
  {{ 0x1.6ad648p-2f, 0x1.6ad62cp-2f}, 0x1.d70290p-1f}, //    14  
  {{ 0x1.6bbb4cp-2f, 0x1.6bbb32p-2f}, 0x1.d71290p-1f}, //    13  
  {{ 0x1.6ca04ap-2f, 0x1.6ca030p-2f}, 0x1.d7228ep-1f}, //    13  
  {{ 0x1.6d855ep-2f, 0x1.6d8544p-2f}, 0x1.d7328cp-1f}, //    13  
  {{ 0x1.6e6aa6p-2f, 0x1.6e6a8cp-2f}, 0x1.d7428cp-1f}, //    13  
  {{ 0x1.6f5004p-2f, 0x1.6f4fe8p-2f}, 0x1.d7528cp-1f}, //    14  
  {{ 0x1.70355ap-2f, 0x1.703540p-2f}, 0x1.d7628ap-1f}, //    13  
  {{ 0x1.711ac6p-2f, 0x1.711aacp-2f}, 0x1.d77288p-1f}, //    13  
  {{ 0x1.720066p-2f, 0x1.72004cp-2f}, 0x1.d78288p-1f}, //    13  
  {{ 0x1.72e61cp-2f, 0x1.72e602p-2f}, 0x1.d79288p-1f}, //    13  
  {{ 0x1.73cbcap-2f, 0x1.73cbb0p-2f}, 0x1.d7a286p-1f}, //    13  
  {{ 0x1.74b190p-2f, 0x1.74b176p-2f}, 0x1.d7b284p-1f}, //    13  
  {{ 0x1.759788p-2f, 0x1.75976ep-2f}, 0x1.d7c284p-1f}, //    13  
  {{ 0x1.767d96p-2f, 0x1.767d7cp-2f}, 0x1.d7d284p-1f}, //    13  
  {{ 0x1.77639ep-2f, 0x1.776382p-2f}, 0x1.d7e282p-1f}, //    14  
  {{ 0x1.7849bcp-2f, 0x1.7849a0p-2f}, 0x1.d7f280p-1f}, //    14  
  {{ 0x1.79300cp-2f, 0x1.792ff0p-2f}, 0x1.d80280p-1f}, //    14  
  {{ 0x1.7a1672p-2f, 0x1.7a1658p-2f}, 0x1.d81280p-1f}, //    13  
  {{ 0x1.7afcd2p-2f, 0x1.7afcb6p-2f}, 0x1.d8227ep-1f}, //    14  
  {{ 0x1.7be348p-2f, 0x1.7be32cp-2f}, 0x1.d8327cp-1f}, //    14  
  {{ 0x1.7cc9f0p-2f, 0x1.7cc9d6p-2f}, 0x1.d8427cp-1f}, //    13  
  {{ 0x1.7db0b0p-2f, 0x1.7db094p-2f}, 0x1.d8527cp-1f}, //    14  
  {{ 0x1.7e9768p-2f, 0x1.7e974ep-2f}, 0x1.d8627ap-1f}, //    13  
  {{ 0x1.7f7e36p-2f, 0x1.7f7e1cp-2f}, 0x1.d87278p-1f}, //    13  
  {{ 0x1.806538p-2f, 0x1.80651cp-2f}, 0x1.d88278p-1f}, //    14  
  {{ 0x1.814c50p-2f, 0x1.814c34p-2f}, 0x1.d89278p-1f}, //    14  
  {{ 0x1.823360p-2f, 0x1.823346p-2f}, 0x1.d8a276p-1f}, //    13  
  {{ 0x1.831a88p-2f, 0x1.831a6cp-2f}, 0x1.d8b274p-1f}, //    14  
  {{ 0x1.8401e2p-2f, 0x1.8401c6p-2f}, 0x1.d8c274p-1f}, //    14  
  {{ 0x1.84e952p-2f, 0x1.84e936p-2f}, 0x1.d8d274p-1f}, //    14  
  {{ 0x1.85d0bap-2f, 0x1.85d0a0p-2f}, 0x1.d8e272p-1f}, //    13  
  {{ 0x1.86b83ap-2f, 0x1.86b820p-2f}, 0x1.d8f270p-1f}, //    13  
  {{ 0x1.879feep-2f, 0x1.879fd2p-2f}, 0x1.d90270p-1f}, //    14  
  {{ 0x1.8887b6p-2f, 0x1.88879cp-2f}, 0x1.d91270p-1f}, //    13  
  {{ 0x1.896f78p-2f, 0x1.896f5ep-2f}, 0x1.d9226ep-1f}, //    13  
  {{ 0x1.8a5750p-2f, 0x1.8a5736p-2f}, 0x1.d9326cp-1f}, //    13  
  {{ 0x1.8b3f5cp-2f, 0x1.8b3f40p-2f}, 0x1.d9426cp-1f}, //    14  
  {{ 0x1.8c277ep-2f, 0x1.8c2762p-2f}, 0x1.d9526cp-1f}, //    14  
  {{ 0x1.8d0f98p-2f, 0x1.8d0f7cp-2f}, 0x1.d9626ap-1f}, //    14  
  {{ 0x1.8df7c8p-2f, 0x1.8df7aep-2f}, 0x1.d97268p-1f}, //    13  
  {{ 0x1.8ee02cp-2f, 0x1.8ee012p-2f}, 0x1.d98268p-1f}, //    13  
  {{ 0x1.8fc8a8p-2f, 0x1.8fc88cp-2f}, 0x1.d99268p-1f}, //    14  
  {{ 0x1.90b11ap-2f, 0x1.90b100p-2f}, 0x1.d9a266p-1f}, //    13  
  {{ 0x1.9199a4p-2f, 0x1.919988p-2f}, 0x1.d9b264p-1f}, //    14  
  {{ 0x1.928260p-2f, 0x1.928246p-2f}, 0x1.d9c264p-1f}, //    13  
  {{ 0x1.936b34p-2f, 0x1.936b18p-2f}, 0x1.d9d264p-1f}, //    14  
  {{ 0x1.945400p-2f, 0x1.9453e6p-2f}, 0x1.d9e262p-1f}, //    13  
  {{ 0x1.953ce2p-2f, 0x1.953cc8p-2f}, 0x1.d9f260p-1f}, //    13  
  {{ 0x1.9625f8p-2f, 0x1.9625dep-2f}, 0x1.da0260p-1f}, //    13  
  {{ 0x1.970f24p-2f, 0x1.970f08p-2f}, 0x1.da1260p-1f}, //    14  
  {{ 0x1.97f84ap-2f, 0x1.97f82ep-2f}, 0x1.da225ep-1f}, //    14  
  {{ 0x1.98e184p-2f, 0x1.98e16ap-2f}, 0x1.da325cp-1f}, //    13  
  {{ 0x1.99caf2p-2f, 0x1.99cad8p-2f}, 0x1.da425cp-1f}, //    13  
  {{ 0x1.9ab478p-2f, 0x1.9ab45cp-2f}, 0x1.da525cp-1f}, //    14  
  {{ 0x1.9b9df6p-2f, 0x1.9b9ddap-2f}, 0x1.da625ap-1f}, //    14  
  {{ 0x1.9c878ap-2f, 0x1.9c876ep-2f}, 0x1.da7258p-1f}, //    14  
  {{ 0x1.9d7150p-2f, 0x1.9d7136p-2f}, 0x1.da8258p-1f}, //    13  
  {{ 0x1.9e5b2ep-2f, 0x1.9e5b14p-2f}, 0x1.da9258p-1f}, //    13  
  {{ 0x1.9f4506p-2f, 0x1.9f44eap-2f}, 0x1.daa256p-1f}, //    14  
  {{ 0x1.a02ef2p-2f, 0x1.a02ed8p-2f}, 0x1.dab254p-1f}, //    13  
  {{ 0x1.a11912p-2f, 0x1.a118f8p-2f}, 0x1.dac254p-1f}, //    13  
  {{ 0x1.a2034ap-2f, 0x1.a2032ep-2f}, 0x1.dad254p-1f}, //    14  
  {{ 0x1.a2ed7ap-2f, 0x1.a2ed5ep-2f}, 0x1.dae252p-1f}, //    14  
  {{ 0x1.a3d7c0p-2f, 0x1.a3d7a4p-2f}, 0x1.daf250p-1f}, //    14  
  {{ 0x1.a4c238p-2f, 0x1.a4c21ep-2f}, 0x1.db0250p-1f}, //    13  
  {{ 0x1.a5acc8p-2f, 0x1.a5acacp-2f}, 0x1.db1250p-1f}, //    14  
  {{ 0x1.a69750p-2f, 0x1.a69736p-2f}, 0x1.db224ep-1f}, //    13  
  {{ 0x1.a781f0p-2f, 0x1.a781d4p-2f}, 0x1.db324cp-1f}, //    14  
  {{ 0x1.a86cc2p-2f, 0x1.a86ca8p-2f}, 0x1.db424cp-1f}, //    13  
  {{ 0x1.a957acp-2f, 0x1.a95790p-2f}, 0x1.db524cp-1f}, //    14  
  {{ 0x1.aa428cp-2f, 0x1.aa4272p-2f}, 0x1.db624ap-1f}, //    13  
  {{ 0x1.ab2d84p-2f, 0x1.ab2d6ap-2f}, 0x1.db7248p-1f}, //    13  
  {{ 0x1.ac18b0p-2f, 0x1.ac1896p-2f}, 0x1.db8248p-1f}, //    13  
  {{ 0x1.ad03f2p-2f, 0x1.ad03d8p-2f}, 0x1.db9248p-1f}, //    13  
  {{ 0x1.adef2ep-2f, 0x1.adef12p-2f}, 0x1.dba246p-1f}, //    14  
  {{ 0x1.aeda7ep-2f, 0x1.aeda62p-2f}, 0x1.dbb244p-1f}, //    14  
  {{ 0x1.afc604p-2f, 0x1.afc5e8p-2f}, 0x1.dbc244p-1f}, //    14  
  {{ 0x1.b0b19ep-2f, 0x1.b0b182p-2f}, 0x1.dbd244p-1f}, //    14  
  {{ 0x1.b19d32p-2f, 0x1.b19d16p-2f}, 0x1.dbe242p-1f}, //    14  
  {{ 0x1.b288dcp-2f, 0x1.b288c0p-2f}, 0x1.dbf240p-1f}, //    14  
  {{ 0x1.b374bap-2f, 0x1.b3749ep-2f}, 0x1.dc0240p-1f}, //    14  
  {{ 0x1.b460aep-2f, 0x1.b46094p-2f}, 0x1.dc1240p-1f}, //    13  
  {{ 0x1.b54c9cp-2f, 0x1.b54c80p-2f}, 0x1.dc223ep-1f}, //    14  
  {{ 0x1.b638a0p-2f, 0x1.b63884p-2f}, 0x1.dc323cp-1f}, //    14  
  {{ 0x1.b724d6p-2f, 0x1.b724bap-2f}, 0x1.dc423cp-1f}, //    14  
  {{ 0x1.b81124p-2f, 0x1.b81108p-2f}, 0x1.dc523cp-1f}, //    14  
  {{ 0x1.b8fd6ap-2f, 0x1.b8fd4ep-2f}, 0x1.dc623ap-1f}, //    14  
  {{ 0x1.b9e9c6p-2f, 0x1.b9e9acp-2f}, 0x1.dc7238p-1f}, //    13  
  {{ 0x1.bad658p-2f, 0x1.bad63cp-2f}, 0x1.dc8238p-1f}, //    14  
  {{ 0x1.bbc2fep-2f, 0x1.bbc2e2p-2f}, 0x1.dc9238p-1f}, //    14  
  {{ 0x1.bcaf9ep-2f, 0x1.bcaf82p-2f}, 0x1.dca236p-1f}, //    14  
  {{ 0x1.bd9c54p-2f, 0x1.bd9c38p-2f}, 0x1.dcb234p-1f}, //    14  
  {{ 0x1.be893ep-2f, 0x1.be8922p-2f}, 0x1.dcc234p-1f}, //    14  
  {{ 0x1.bf763ep-2f, 0x1.bf7622p-2f}, 0x1.dcd234p-1f}, //    14  
  {{ 0x1.c06336p-2f, 0x1.c0631cp-2f}, 0x1.dce232p-1f}, //    13  
  {{ 0x1.c15046p-2f, 0x1.c1502ap-2f}, 0x1.dcf230p-1f}, //    14  
  {{ 0x1.c23d8ap-2f, 0x1.c23d6ep-2f}, 0x1.dd0230p-1f}, //    14  
  {{ 0x1.c32ae2p-2f, 0x1.c32ac8p-2f}, 0x1.dd1230p-1f}, //    13  
  {{ 0x1.c41836p-2f, 0x1.c4181ap-2f}, 0x1.dd222ep-1f}, //    14  
  {{ 0x1.c5059ep-2f, 0x1.c50582p-2f}, 0x1.dd322cp-1f}, //    14  
  {{ 0x1.c5f33ap-2f, 0x1.c5f31ep-2f}, 0x1.dd422cp-1f}, //    14  
  {{ 0x1.c6e0eep-2f, 0x1.c6e0d2p-2f}, 0x1.dd522cp-1f}, //    14  
  {{ 0x1.c7ce9ap-2f, 0x1.c7ce7ep-2f}, 0x1.dd622ap-1f}, //    14  
  {{ 0x1.c8bc5cp-2f, 0x1.c8bc40p-2f}, 0x1.dd7228p-1f}, //    14  
  {{ 0x1.c9aa52p-2f, 0x1.c9aa36p-2f}, 0x1.dd8228p-1f}, //    14  
  {{ 0x1.ca985ep-2f, 0x1.ca9842p-2f}, 0x1.dd9228p-1f}, //    14  
  {{ 0x1.cb8664p-2f, 0x1.cb8648p-2f}, 0x1.dda226p-1f}, //    14  
  {{ 0x1.cc747ep-2f, 0x1.cc7464p-2f}, 0x1.ddb224p-1f}, //    13  
  {{ 0x1.cd62cep-2f, 0x1.cd62b2p-2f}, 0x1.ddc224p-1f}, //    14  
  {{ 0x1.ce5134p-2f, 0x1.ce5118p-2f}, 0x1.ddd224p-1f}, //    14  
  {{ 0x1.cf3f94p-2f, 0x1.cf3f78p-2f}, 0x1.dde222p-1f}, //    14  
  {{ 0x1.d02e08p-2f, 0x1.d02decp-2f}, 0x1.ddf220p-1f}, //    14  
  {{ 0x1.d11cb2p-2f, 0x1.d11c96p-2f}, 0x1.de0220p-1f}, //    14  
  {{ 0x1.d20b72p-2f, 0x1.d20b56p-2f}, 0x1.de1220p-1f}, //    14  
  {{ 0x1.d2fa2ap-2f, 0x1.d2fa0ep-2f}, 0x1.de221ep-1f}, //    14  
  {{ 0x1.d3e8f8p-2f, 0x1.d3e8dcp-2f}, 0x1.de321cp-1f}, //    14  
  {{ 0x1.d4d7fcp-2f, 0x1.d4d7e0p-2f}, 0x1.de421cp-1f}, //    14  
  {{ 0x1.d5c714p-2f, 0x1.d5c6f8p-2f}, 0x1.de521cp-1f}, //    14  
  {{ 0x1.d6b626p-2f, 0x1.d6b60ap-2f}, 0x1.de621ap-1f}, //    14  
  {{ 0x1.d7a54ep-2f, 0x1.d7a532p-2f}, 0x1.de7218p-1f}, //    14  
  {{ 0x1.d894acp-2f, 0x1.d89490p-2f}, 0x1.de8218p-1f}, //    14  
  {{ 0x1.d9841ep-2f, 0x1.d98402p-2f}, 0x1.de9218p-1f}, //    14  
  {{ 0x1.da738ap-2f, 0x1.da736ep-2f}, 0x1.dea216p-1f}, //    14  
  {{ 0x1.db630cp-2f, 0x1.db62f0p-2f}, 0x1.deb214p-1f}, //    14  
  {{ 0x1.dc52c2p-2f, 0x1.dc52a6p-2f}, 0x1.dec214p-1f}, //    14  
  {{ 0x1.dd428ep-2f, 0x1.dd4272p-2f}, 0x1.ded214p-1f}, //    14  
  {{ 0x1.de3254p-2f, 0x1.de3238p-2f}, 0x1.dee212p-1f}, //    14  
  {{ 0x1.df2230p-2f, 0x1.df2214p-2f}, 0x1.def210p-1f}, //    14  
  {{ 0x1.e01240p-2f, 0x1.e01224p-2f}, 0x1.df0210p-1f}, //    14  
  {{ 0x1.e10266p-2f, 0x1.e1024ap-2f}, 0x1.df1210p-1f}, //    14  
  {{ 0x1.e1f284p-2f, 0x1.e1f268p-2f}, 0x1.df220ep-1f}, //    14  
  {{ 0x1.e2e2bap-2f, 0x1.e2e29ep-2f}, 0x1.df320cp-1f}, //    14  
  {{ 0x1.e3d324p-2f, 0x1.e3d308p-2f}, 0x1.df420cp-1f}, //    14  
  {{ 0x1.e4c3a4p-2f, 0x1.e4c388p-2f}, 0x1.df520cp-1f}, //    14  
  {{ 0x1.e5b41ep-2f, 0x1.e5b400p-2f}, 0x1.df620ap-1f}, //    15  
  {{ 0x1.e6a4acp-2f, 0x1.e6a490p-2f}, 0x1.df7208p-1f}, //    14  
  {{ 0x1.e79570p-2f, 0x1.e79554p-2f}, 0x1.df8208p-1f}, //    14  
  {{ 0x1.e8864ap-2f, 0x1.e8862ep-2f}, 0x1.df9208p-1f}, //    14  
  {{ 0x1.e9771cp-2f, 0x1.e97700p-2f}, 0x1.dfa206p-1f}, //    14  
  {{ 0x1.ea6806p-2f, 0x1.ea67eap-2f}, 0x1.dfb204p-1f}, //    14  
  {{ 0x1.eb5924p-2f, 0x1.eb5908p-2f}, 0x1.dfc204p-1f}, //    14  
  {{ 0x1.ec4a58p-2f, 0x1.ec4a3cp-2f}, 0x1.dfd204p-1f}, //    14  
  {{ 0x1.ed3b84p-2f, 0x1.ed3b68p-2f}, 0x1.dfe202p-1f}, //    14  
  {{ 0x1.ee2cc6p-2f, 0x1.ee2caap-2f}, 0x1.dff200p-1f}, //    14  
  {{ 0x1.ef1e3ep-2f, 0x1.ef1e22p-2f}, 0x1.e00200p-1f}, //    14  
  {{ 0x1.f00fccp-2f, 0x1.f00fb0p-2f}, 0x1.e01200p-1f}, //    14  
  {{ 0x1.f10152p-2f, 0x1.f10136p-2f}, 0x1.e021fep-1f}, //    14  
  {{ 0x1.f1f2f0p-2f, 0x1.f1f2d4p-2f}, 0x1.e031fcp-1f}, //    14  
  {{ 0x1.f2e4c2p-2f, 0x1.f2e4a4p-2f}, 0x1.e041fcp-1f}, //    15  
  {{ 0x1.f3d6aap-2f, 0x1.f3d68cp-2f}, 0x1.e051fcp-1f}, //    15  
  {{ 0x1.f4c88ap-2f, 0x1.f4c86ep-2f}, 0x1.e061fap-1f}, //    14  
  {{ 0x1.f5ba80p-2f, 0x1.f5ba64p-2f}, 0x1.e071f8p-1f}, //    14  
  {{ 0x1.f6acacp-2f, 0x1.f6ac90p-2f}, 0x1.e081f8p-1f}, //    14  
  {{ 0x1.f79eeep-2f, 0x1.f79ed2p-2f}, 0x1.e091f8p-1f}, //    14  
  {{ 0x1.f89128p-2f, 0x1.f8910cp-2f}, 0x1.e0a1f6p-1f}, //    14  
  {{ 0x1.f9837ap-2f, 0x1.f9835ep-2f}, 0x1.e0b1f4p-1f}, //    14  
  {{ 0x1.fa7600p-2f, 0x1.fa75e2p-2f}, 0x1.e0c1f4p-1f}, //    15  
  {{ 0x1.fb689cp-2f, 0x1.fb687ep-2f}, 0x1.e0d1f4p-1f}, //    15  
  {{ 0x1.fc5b30p-2f, 0x1.fc5b14p-2f}, 0x1.e0e1f2p-1f}, //    14  
  {{ 0x1.fd4ddap-2f, 0x1.fd4dbep-2f}, 0x1.e0f1f0p-1f}, //    14  
  {{ 0x1.fe40bap-2f, 0x1.fe409ep-2f}, 0x1.e101f0p-1f}, //    14  
  {{ 0x1.ff33b0p-2f, 0x1.ff3394p-2f}, 0x1.e111f0p-1f}, //    14  
  {{ 0x1.001350p-1f, 0x1.001342p-1f}, 0x1.e121eep-1f}, //     7  
  {{ 0x1.008cd2p-1f, 0x1.008cc4p-1f}, 0x1.e131ecp-1f}, //     7  
  {{ 0x1.01066ep-1f, 0x1.010662p-1f}, 0x1.e141ecp-1f}, //     6  
  {{ 0x1.018018p-1f, 0x1.01800ap-1f}, 0x1.e151ecp-1f}, //     7  
  {{ 0x1.01f9bcp-1f, 0x1.01f9aep-1f}, 0x1.e161eap-1f}, //     7  
  {{ 0x1.02736cp-1f, 0x1.02735ep-1f}, 0x1.e171e8p-1f}, //     7  
  {{ 0x1.02ed36p-1f, 0x1.02ed28p-1f}, 0x1.e181e8p-1f}, //     7  
  {{ 0x1.03670ap-1f, 0x1.0366fep-1f}, 0x1.e191e8p-1f}, //     6  
  {{ 0x1.03e0dcp-1f, 0x1.03e0cep-1f}, 0x1.e1a1e6p-1f}, //     7  
  {{ 0x1.045ab8p-1f, 0x1.045aacp-1f}, 0x1.e1b1e4p-1f}, //     6  
  {{ 0x1.04d4b0p-1f, 0x1.04d4a2p-1f}, 0x1.e1c1e4p-1f}, //     7  
  {{ 0x1.054eb2p-1f, 0x1.054ea6p-1f}, 0x1.e1d1e4p-1f}, //     6  
  {{ 0x1.05c8b2p-1f, 0x1.05c8a4p-1f}, 0x1.e1e1e2p-1f}, //     7  
  {{ 0x1.0642bcp-1f, 0x1.0642aep-1f}, 0x1.e1f1e0p-1f}, //     7  
  {{ 0x1.06bce0p-1f, 0x1.06bcd2p-1f}, 0x1.e201e0p-1f}, //     7  
  {{ 0x1.073710p-1f, 0x1.073702p-1f}, 0x1.e211e0p-1f}, //     7  
  {{ 0x1.07b13ap-1f, 0x1.07b12ep-1f}, 0x1.e221dep-1f}, //     6  
  {{ 0x1.082b72p-1f, 0x1.082b64p-1f}, 0x1.e231dcp-1f}, //     7  
  {{ 0x1.08a5c4p-1f, 0x1.08a5b6p-1f}, 0x1.e241dcp-1f}, //     7  
  {{ 0x1.092020p-1f, 0x1.092014p-1f}, 0x1.e251dcp-1f}, //     6  
  {{ 0x1.099a7ap-1f, 0x1.099a6cp-1f}, 0x1.e261dap-1f}, //     7  
  {{ 0x1.0a14dep-1f, 0x1.0a14d0p-1f}, 0x1.e271d8p-1f}, //     7  
  {{ 0x1.0a8f5cp-1f, 0x1.0a8f50p-1f}, 0x1.e281d8p-1f}, //     6  
  {{ 0x1.0b09e6p-1f, 0x1.0b09dap-1f}, 0x1.e291d8p-1f}, //     6  
  {{ 0x1.0b846cp-1f, 0x1.0b8460p-1f}, 0x1.e2a1d6p-1f}, //     6  
  {{ 0x1.0bfefep-1f, 0x1.0bfef0p-1f}, 0x1.e2b1d4p-1f}, //     7  
  {{ 0x1.0c79aap-1f, 0x1.0c799cp-1f}, 0x1.e2c1d4p-1f}, //     7  
  {{ 0x1.0cf462p-1f, 0x1.0cf454p-1f}, 0x1.e2d1d4p-1f}, //     7  
  {{ 0x1.0d6f14p-1f, 0x1.0d6f08p-1f}, 0x1.e2e1d2p-1f}, //     6  
  {{ 0x1.0de9d4p-1f, 0x1.0de9c6p-1f}, 0x1.e2f1d0p-1f}, //     7  
  {{ 0x1.0e64acp-1f, 0x1.0e64a0p-1f}, 0x1.e301d0p-1f}, //     6  
  {{ 0x1.0edf92p-1f, 0x1.0edf84p-1f}, 0x1.e311d0p-1f}, //     7  
  {{ 0x1.0f5a72p-1f, 0x1.0f5a64p-1f}, 0x1.e321cep-1f}, //     7  
  {{ 0x1.0fd55ep-1f, 0x1.0fd550p-1f}, 0x1.e331ccp-1f}, //     7  
  {{ 0x1.105064p-1f, 0x1.105058p-1f}, 0x1.e341ccp-1f}, //     6  
  {{ 0x1.10cb76p-1f, 0x1.10cb6ap-1f}, 0x1.e351ccp-1f}, //     6  
  {{ 0x1.114684p-1f, 0x1.114676p-1f}, 0x1.e361cap-1f}, //     7  
  {{ 0x1.11c19ep-1f, 0x1.11c190p-1f}, 0x1.e371c8p-1f}, //     7  
  {{ 0x1.123cd2p-1f, 0x1.123cc4p-1f}, 0x1.e381c8p-1f}, //     7  
  {{ 0x1.12b810p-1f, 0x1.12b804p-1f}, 0x1.e391c8p-1f}, //     6  
  {{ 0x1.13334cp-1f, 0x1.13333ep-1f}, 0x1.e3a1c6p-1f}, //     7  
  {{ 0x1.13ae92p-1f, 0x1.13ae84p-1f}, 0x1.e3b1c4p-1f}, //     7  
  {{ 0x1.1429f4p-1f, 0x1.1429e6p-1f}, 0x1.e3c1c4p-1f}, //     7  
  {{ 0x1.14a560p-1f, 0x1.14a552p-1f}, 0x1.e3d1c4p-1f}, //     7  
  {{ 0x1.1520c8p-1f, 0x1.1520bcp-1f}, 0x1.e3e1c2p-1f}, //     6  
  {{ 0x1.159c3cp-1f, 0x1.159c2ep-1f}, 0x1.e3f1c0p-1f}, //     7  
  {{ 0x1.1617cap-1f, 0x1.1617bep-1f}, 0x1.e401c0p-1f}, //     6  
  {{ 0x1.169364p-1f, 0x1.169358p-1f}, 0x1.e411c0p-1f}, //     6  
  {{ 0x1.170efap-1f, 0x1.170eeep-1f}, 0x1.e421bep-1f}, //     6  
  {{ 0x1.178a9cp-1f, 0x1.178a8ep-1f}, 0x1.e431bcp-1f}, //     7  
  {{ 0x1.180658p-1f, 0x1.18064ap-1f}, 0x1.e441bcp-1f}, //     7  
  {{ 0x1.18821ep-1f, 0x1.188212p-1f}, 0x1.e451bcp-1f}, //     6  
  {{ 0x1.18fde2p-1f, 0x1.18fdd4p-1f}, 0x1.e461bap-1f}, //     7  
  {{ 0x1.1979b0p-1f, 0x1.1979a4p-1f}, 0x1.e471b8p-1f}, //     6  
  {{ 0x1.19f59ap-1f, 0x1.19f58cp-1f}, 0x1.e481b8p-1f}, //     7  
  {{ 0x1.1a718ep-1f, 0x1.1a7182p-1f}, 0x1.e491b8p-1f}, //     6  
  {{ 0x1.1aed80p-1f, 0x1.1aed72p-1f}, 0x1.e4a1b6p-1f}, //     7  
  {{ 0x1.1b697cp-1f, 0x1.1b696ep-1f}, 0x1.e4b1b4p-1f}, //     7  
  {{ 0x1.1be592p-1f, 0x1.1be584p-1f}, 0x1.e4c1b4p-1f}, //     7  
  {{ 0x1.1c61b4p-1f, 0x1.1c61a6p-1f}, 0x1.e4d1b4p-1f}, //     7  
  {{ 0x1.1cddd2p-1f, 0x1.1cddc4p-1f}, 0x1.e4e1b2p-1f}, //     7  
  {{ 0x1.1d59fcp-1f, 0x1.1d59eep-1f}, 0x1.e4f1b0p-1f}, //     7  
  {{ 0x1.1dd640p-1f, 0x1.1dd632p-1f}, 0x1.e501b0p-1f}, //     7  
  {{ 0x1.1e5290p-1f, 0x1.1e5282p-1f}, 0x1.e511b0p-1f}, //     7  
  {{ 0x1.1ecedap-1f, 0x1.1ececcp-1f}, 0x1.e521aep-1f}, //     7  
  {{ 0x1.1f4b32p-1f, 0x1.1f4b24p-1f}, 0x1.e531acp-1f}, //     7  
  {{ 0x1.1fc7a2p-1f, 0x1.1fc796p-1f}, 0x1.e541acp-1f}, //     6  
  {{ 0x1.204420p-1f, 0x1.204412p-1f}, 0x1.e551acp-1f}, //     7  
  {{ 0x1.20c098p-1f, 0x1.20c08cp-1f}, 0x1.e561aap-1f}, //     6  
  {{ 0x1.213d1ep-1f, 0x1.213d10p-1f}, 0x1.e571a8p-1f}, //     7  
  {{ 0x1.21b9bcp-1f, 0x1.21b9aep-1f}, 0x1.e581a8p-1f}, //     7  
  {{ 0x1.223666p-1f, 0x1.22365ap-1f}, 0x1.e591a8p-1f}, //     6  
  {{ 0x1.22b30ep-1f, 0x1.22b300p-1f}, 0x1.e5a1a6p-1f}, //     7  
  {{ 0x1.232fbep-1f, 0x1.232fb2p-1f}, 0x1.e5b1a4p-1f}, //     6  
  {{ 0x1.23ac8cp-1f, 0x1.23ac7ep-1f}, 0x1.e5c1a4p-1f}, //     7  
  {{ 0x1.242964p-1f, 0x1.242956p-1f}, 0x1.e5d1a4p-1f}, //     7  
  {{ 0x1.24a638p-1f, 0x1.24a62ap-1f}, 0x1.e5e1a2p-1f}, //     7  
  {{ 0x1.252316p-1f, 0x1.252308p-1f}, 0x1.e5f1a0p-1f}, //     7  
  {{ 0x1.25a010p-1f, 0x1.25a004p-1f}, 0x1.e601a0p-1f}, //     6  
  {{ 0x1.261d16p-1f, 0x1.261d08p-1f}, 0x1.e611a0p-1f}, //     7  
  {{ 0x1.269a18p-1f, 0x1.269a0ap-1f}, 0x1.e6219ep-1f}, //     7  
  {{ 0x1.271724p-1f, 0x1.271716p-1f}, 0x1.e6319cp-1f}, //     7  
  {{ 0x1.27944cp-1f, 0x1.27943ep-1f}, 0x1.e6419cp-1f}, //     7  
  {{ 0x1.281180p-1f, 0x1.281172p-1f}, 0x1.e6519cp-1f}, //     7  
  {{ 0x1.288eaep-1f, 0x1.288ea0p-1f}, 0x1.e6619ap-1f}, //     7  
  {{ 0x1.290be8p-1f, 0x1.290bdap-1f}, 0x1.e67198p-1f}, //     7  
  {{ 0x1.29893ep-1f, 0x1.298930p-1f}, 0x1.e68198p-1f}, //     7  
  {{ 0x1.2a069ep-1f, 0x1.2a0692p-1f}, 0x1.e69198p-1f}, //     6  
  {{ 0x1.2a83fcp-1f, 0x1.2a83eep-1f}, 0x1.e6a196p-1f}, //     7  
  {{ 0x1.2b0164p-1f, 0x1.2b0156p-1f}, 0x1.e6b194p-1f}, //     7  
  {{ 0x1.2b7ee6p-1f, 0x1.2b7ed8p-1f}, 0x1.e6c194p-1f}, //     7  
  {{ 0x1.2bfc74p-1f, 0x1.2bfc66p-1f}, 0x1.e6d194p-1f}, //     7  
  {{ 0x1.2c79fep-1f, 0x1.2c79f0p-1f}, 0x1.e6e192p-1f}, //     7  
  {{ 0x1.2cf794p-1f, 0x1.2cf786p-1f}, 0x1.e6f190p-1f}, //     7  
  {{ 0x1.2d7544p-1f, 0x1.2d7536p-1f}, 0x1.e70190p-1f}, //     7  
  {{ 0x1.2df300p-1f, 0x1.2df2f2p-1f}, 0x1.e71190p-1f}, //     7  
  {{ 0x1.2e70b8p-1f, 0x1.2e70aap-1f}, 0x1.e7218ep-1f}, //     7  
  {{ 0x1.2eee7cp-1f, 0x1.2eee6ep-1f}, 0x1.e7318cp-1f}, //     7  
  {{ 0x1.2f6c5ap-1f, 0x1.2f6c4cp-1f}, 0x1.e7418cp-1f}, //     7  
  {{ 0x1.2fea44p-1f, 0x1.2fea36p-1f}, 0x1.e7518cp-1f}, //     7  
  {{ 0x1.306828p-1f, 0x1.30681ap-1f}, 0x1.e7618ap-1f}, //     7  
  {{ 0x1.30e61ap-1f, 0x1.30e60cp-1f}, 0x1.e77188p-1f}, //     7  
  {{ 0x1.316426p-1f, 0x1.316418p-1f}, 0x1.e78188p-1f}, //     7  
  {{ 0x1.31e23cp-1f, 0x1.31e22ep-1f}, 0x1.e79188p-1f}, //     7  
  {{ 0x1.326050p-1f, 0x1.326042p-1f}, 0x1.e7a186p-1f}, //     7  
  {{ 0x1.32de6ep-1f, 0x1.32de60p-1f}, 0x1.e7b184p-1f}, //     7  
  {{ 0x1.335ca8p-1f, 0x1.335c9ap-1f}, 0x1.e7c184p-1f}, //     7  
  {{ 0x1.33daecp-1f, 0x1.33dae0p-1f}, 0x1.e7d184p-1f}, //     6  
  {{ 0x1.34592ep-1f, 0x1.345920p-1f}, 0x1.e7e182p-1f}, //     7  
  {{ 0x1.34d77ap-1f, 0x1.34d76cp-1f}, 0x1.e7f180p-1f}, //     7  
  {{ 0x1.3555e2p-1f, 0x1.3555d4p-1f}, 0x1.e80180p-1f}, //     7  
  {{ 0x1.35d454p-1f, 0x1.35d446p-1f}, 0x1.e81180p-1f}, //     7  
  {{ 0x1.3652c2p-1f, 0x1.3652b4p-1f}, 0x1.e8217ep-1f}, //     7  
  {{ 0x1.36d13cp-1f, 0x1.36d12ep-1f}, 0x1.e8317cp-1f}, //     7  
  {{ 0x1.374fd2p-1f, 0x1.374fc4p-1f}, 0x1.e8417cp-1f}, //     7  
  {{ 0x1.37ce72p-1f, 0x1.37ce64p-1f}, 0x1.e8517cp-1f}, //     7  
  {{ 0x1.384d0ep-1f, 0x1.384d00p-1f}, 0x1.e8617ap-1f}, //     7  
  {{ 0x1.38cbb6p-1f, 0x1.38cba8p-1f}, 0x1.e87178p-1f}, //     7  
  {{ 0x1.394a78p-1f, 0x1.394a6ap-1f}, 0x1.e88178p-1f}, //     7  
  {{ 0x1.39c946p-1f, 0x1.39c938p-1f}, 0x1.e89178p-1f}, //     7  
  {{ 0x1.3a4810p-1f, 0x1.3a4802p-1f}, 0x1.e8a176p-1f}, //     7  
  {{ 0x1.3ac6e6p-1f, 0x1.3ac6d8p-1f}, 0x1.e8b174p-1f}, //     7  
  {{ 0x1.3b45d6p-1f, 0x1.3b45c8p-1f}, 0x1.e8c174p-1f}, //     7  
  {{ 0x1.3bc4d2p-1f, 0x1.3bc4c4p-1f}, 0x1.e8d174p-1f}, //     7  
  {{ 0x1.3c43cap-1f, 0x1.3c43bcp-1f}, 0x1.e8e172p-1f}, //     7  
  {{ 0x1.3cc2cep-1f, 0x1.3cc2c0p-1f}, 0x1.e8f170p-1f}, //     7  
  {{ 0x1.3d41ecp-1f, 0x1.3d41dep-1f}, 0x1.e90170p-1f}, //     7  
  {{ 0x1.3dc116p-1f, 0x1.3dc108p-1f}, 0x1.e91170p-1f}, //     7  
  {{ 0x1.3e403cp-1f, 0x1.3e402ep-1f}, 0x1.e9216ep-1f}, //     7  
  {{ 0x1.3ebf6cp-1f, 0x1.3ebf5ep-1f}, 0x1.e9316cp-1f}, //     7  
  {{ 0x1.3f3eb8p-1f, 0x1.3f3eaap-1f}, 0x1.e9416cp-1f}, //     7  
  {{ 0x1.3fbe10p-1f, 0x1.3fbe02p-1f}, 0x1.e9516cp-1f}, //     7  
  {{ 0x1.403d64p-1f, 0x1.403d56p-1f}, 0x1.e9616ap-1f}, //     7  
  {{ 0x1.40bcc2p-1f, 0x1.40bcb4p-1f}, 0x1.e97168p-1f}, //     7  
  {{ 0x1.413c3ep-1f, 0x1.413c30p-1f}, 0x1.e98168p-1f}, //     7  
  {{ 0x1.41bbc2p-1f, 0x1.41bbb4p-1f}, 0x1.e99168p-1f}, //     7  
  {{ 0x1.423b44p-1f, 0x1.423b36p-1f}, 0x1.e9a166p-1f}, //     7  
  {{ 0x1.42bad0p-1f, 0x1.42bac2p-1f}, 0x1.e9b164p-1f}, //     7  
  {{ 0x1.433a78p-1f, 0x1.433a6ap-1f}, 0x1.e9c164p-1f}, //     7  
  {{ 0x1.43ba2cp-1f, 0x1.43ba1ep-1f}, 0x1.e9d164p-1f}, //     7  
  {{ 0x1.4439dcp-1f, 0x1.4439cep-1f}, 0x1.e9e162p-1f}, //     7  
  {{ 0x1.44b996p-1f, 0x1.44b988p-1f}, 0x1.e9f160p-1f}, //     7  
  {{ 0x1.45396cp-1f, 0x1.45395ep-1f}, 0x1.ea0160p-1f}, //     7  
  {{ 0x1.45b94ep-1f, 0x1.45b940p-1f}, 0x1.ea1160p-1f}, //     7  
  {{ 0x1.46392ap-1f, 0x1.46391cp-1f}, 0x1.ea215ep-1f}, //     7  
  {{ 0x1.46b912p-1f, 0x1.46b904p-1f}, 0x1.ea315cp-1f}, //     7  
  {{ 0x1.473916p-1f, 0x1.473908p-1f}, 0x1.ea415cp-1f}, //     7  
  {{ 0x1.47b926p-1f, 0x1.47b918p-1f}, 0x1.ea515cp-1f}, //     7  
  {{ 0x1.483930p-1f, 0x1.483922p-1f}, 0x1.ea615ap-1f}, //     7  
  {{ 0x1.48b948p-1f, 0x1.48b93ap-1f}, 0x1.ea7158p-1f}, //     7  
  {{ 0x1.49397ap-1f, 0x1.49396cp-1f}, 0x1.ea8158p-1f}, //     7  
  {{ 0x1.49b9b6p-1f, 0x1.49b9a8p-1f}, 0x1.ea9158p-1f}, //     7  
  {{ 0x1.4a39f0p-1f, 0x1.4a39e2p-1f}, 0x1.eaa156p-1f}, //     7  
  {{ 0x1.4aba34p-1f, 0x1.4aba26p-1f}, 0x1.eab154p-1f}, //     7  
  {{ 0x1.4b3a94p-1f, 0x1.4b3a86p-1f}, 0x1.eac154p-1f}, //     7  
  {{ 0x1.4bbb00p-1f, 0x1.4bbaf2p-1f}, 0x1.ead154p-1f}, //     7  
  {{ 0x1.4c3b66p-1f, 0x1.4c3b58p-1f}, 0x1.eae152p-1f}, //     7  
  {{ 0x1.4cbbd8p-1f, 0x1.4cbbcap-1f}, 0x1.eaf150p-1f}, //     7  
  {{ 0x1.4d3c66p-1f, 0x1.4d3c58p-1f}, 0x1.eb0150p-1f}, //     7  
  {{ 0x1.4dbd00p-1f, 0x1.4dbcf2p-1f}, 0x1.eb1150p-1f}, //     7  
  {{ 0x1.4e3d94p-1f, 0x1.4e3d86p-1f}, 0x1.eb214ep-1f}, //     7  
  {{ 0x1.4ebe34p-1f, 0x1.4ebe26p-1f}, 0x1.eb314cp-1f}, //     7  
  {{ 0x1.4f3ef0p-1f, 0x1.4f3ee2p-1f}, 0x1.eb414cp-1f}, //     7  
  {{ 0x1.4fbfb8p-1f, 0x1.4fbfaap-1f}, 0x1.eb514cp-1f}, //     7  
  {{ 0x1.50407ap-1f, 0x1.50406cp-1f}, 0x1.eb614ap-1f}, //     7  
  {{ 0x1.50c14ap-1f, 0x1.50c13cp-1f}, 0x1.eb7148p-1f}, //     7  
  {{ 0x1.514234p-1f, 0x1.514224p-1f}, 0x1.eb8148p-1f}, //     8  
  {{ 0x1.51c328p-1f, 0x1.51c31ap-1f}, 0x1.eb9148p-1f}, //     7  
  {{ 0x1.52441ap-1f, 0x1.52440cp-1f}, 0x1.eba146p-1f}, //     7  
  {{ 0x1.52c516p-1f, 0x1.52c508p-1f}, 0x1.ebb144p-1f}, //     7  
  {{ 0x1.53462ep-1f, 0x1.534620p-1f}, 0x1.ebc144p-1f}, //     7  
  {{ 0x1.53c752p-1f, 0x1.53c744p-1f}, 0x1.ebd144p-1f}, //     7  
  {{ 0x1.544870p-1f, 0x1.544862p-1f}, 0x1.ebe142p-1f}, //     7  
  {{ 0x1.54c99ap-1f, 0x1.54c98cp-1f}, 0x1.ebf140p-1f}, //     7  
  {{ 0x1.554ae0p-1f, 0x1.554ad2p-1f}, 0x1.ec0140p-1f}, //     7  
  {{ 0x1.55cc32p-1f, 0x1.55cc24p-1f}, 0x1.ec1140p-1f}, //     7  
  {{ 0x1.564d80p-1f, 0x1.564d72p-1f}, 0x1.ec213ep-1f}, //     7  
  {{ 0x1.56ced8p-1f, 0x1.56cecap-1f}, 0x1.ec313cp-1f}, //     7  
  {{ 0x1.57504cp-1f, 0x1.57503ep-1f}, 0x1.ec413cp-1f}, //     7  
  {{ 0x1.57d1ccp-1f, 0x1.57d1bep-1f}, 0x1.ec513cp-1f}, //     7  
  {{ 0x1.585348p-1f, 0x1.58533ap-1f}, 0x1.ec613ap-1f}, //     7  
  {{ 0x1.58d4cep-1f, 0x1.58d4c0p-1f}, 0x1.ec7138p-1f}, //     7  
  {{ 0x1.595670p-1f, 0x1.595662p-1f}, 0x1.ec8138p-1f}, //     7  
  {{ 0x1.59d81ep-1f, 0x1.59d810p-1f}, 0x1.ec9138p-1f}, //     7  
  {{ 0x1.5a59c8p-1f, 0x1.5a59bap-1f}, 0x1.eca136p-1f}, //     7  
  {{ 0x1.5adb7cp-1f, 0x1.5adb6ep-1f}, 0x1.ecb134p-1f}, //     7  
  {{ 0x1.5b5d4ep-1f, 0x1.5b5d3ep-1f}, 0x1.ecc134p-1f}, //     8  
  {{ 0x1.5bdf2ap-1f, 0x1.5bdf1cp-1f}, 0x1.ecd134p-1f}, //     7  
  {{ 0x1.5c6100p-1f, 0x1.5c60f2p-1f}, 0x1.ece132p-1f}, //     7  
  {{ 0x1.5ce2e4p-1f, 0x1.5ce2d6p-1f}, 0x1.ecf130p-1f}, //     7  
  {{ 0x1.5d64e2p-1f, 0x1.5d64d4p-1f}, 0x1.ed0130p-1f}, //     7  
  {{ 0x1.5de6ecp-1f, 0x1.5de6dep-1f}, 0x1.ed1130p-1f}, //     7  
  {{ 0x1.5e68f2p-1f, 0x1.5e68e4p-1f}, 0x1.ed212ep-1f}, //     7  
  {{ 0x1.5eeb04p-1f, 0x1.5eeaf6p-1f}, 0x1.ed312cp-1f}, //     7  
  {{ 0x1.5f6d30p-1f, 0x1.5f6d22p-1f}, 0x1.ed412cp-1f}, //     7  
  {{ 0x1.5fef6ap-1f, 0x1.5fef5ap-1f}, 0x1.ed512cp-1f}, //     8  
  {{ 0x1.60719cp-1f, 0x1.60718ep-1f}, 0x1.ed612ap-1f}, //     7  
  {{ 0x1.60f3dcp-1f, 0x1.60f3cep-1f}, 0x1.ed7128p-1f}, //     7  
  {{ 0x1.617638p-1f, 0x1.61762ap-1f}, 0x1.ed8128p-1f}, //     7  
  {{ 0x1.61f89ep-1f, 0x1.61f890p-1f}, 0x1.ed9128p-1f}, //     7  
  {{ 0x1.627b00p-1f, 0x1.627af2p-1f}, 0x1.eda126p-1f}, //     7  
  {{ 0x1.62fd6ep-1f, 0x1.62fd60p-1f}, 0x1.edb124p-1f}, //     7  
  {{ 0x1.637ff8p-1f, 0x1.637feap-1f}, 0x1.edc124p-1f}, //     7  
  {{ 0x1.64028cp-1f, 0x1.64027ep-1f}, 0x1.edd124p-1f}, //     7  
  {{ 0x1.64851cp-1f, 0x1.64850ep-1f}, 0x1.ede122p-1f}, //     7  
  {{ 0x1.6507b8p-1f, 0x1.6507aap-1f}, 0x1.edf120p-1f}, //     7  
  {{ 0x1.658a70p-1f, 0x1.658a62p-1f}, 0x1.ee0120p-1f}, //     7  
  {{ 0x1.660d34p-1f, 0x1.660d26p-1f}, 0x1.ee1120p-1f}, //     7  
  {{ 0x1.668ff2p-1f, 0x1.668fe4p-1f}, 0x1.ee211ep-1f}, //     7  
  {{ 0x1.6712bcp-1f, 0x1.6712aep-1f}, 0x1.ee311cp-1f}, //     7  
  {{ 0x1.6795a2p-1f, 0x1.679594p-1f}, 0x1.ee411cp-1f}, //     7  
  {{ 0x1.681894p-1f, 0x1.681886p-1f}, 0x1.ee511cp-1f}, //     7  
  {{ 0x1.689b82p-1f, 0x1.689b72p-1f}, 0x1.ee611ap-1f}, //     8  
  {{ 0x1.691e7ap-1f, 0x1.691e6cp-1f}, 0x1.ee7118p-1f}, //     7  
  {{ 0x1.69a18ep-1f, 0x1.69a180p-1f}, 0x1.ee8118p-1f}, //     7  
  {{ 0x1.6a24aep-1f, 0x1.6a24a0p-1f}, 0x1.ee9118p-1f}, //     7  
  {{ 0x1.6aa7cap-1f, 0x1.6aa7bap-1f}, 0x1.eea116p-1f}, //     8  
  {{ 0x1.6b2af0p-1f, 0x1.6b2ae2p-1f}, 0x1.eeb114p-1f}, //     7  
  {{ 0x1.6bae32p-1f, 0x1.6bae24p-1f}, 0x1.eec114p-1f}, //     7  
  {{ 0x1.6c3182p-1f, 0x1.6c3172p-1f}, 0x1.eed114p-1f}, //     8  
  {{ 0x1.6cb4cap-1f, 0x1.6cb4bcp-1f}, 0x1.eee112p-1f}, //     7  
  {{ 0x1.6d3820p-1f, 0x1.6d3812p-1f}, 0x1.eef110p-1f}, //     7  
  {{ 0x1.6dbb92p-1f, 0x1.6dbb82p-1f}, 0x1.ef0110p-1f}, //     8  
  {{ 0x1.6e3f0ep-1f, 0x1.6e3f00p-1f}, 0x1.ef1110p-1f}, //     7  
  {{ 0x1.6ec286p-1f, 0x1.6ec278p-1f}, 0x1.ef210ep-1f}, //     7  
  {{ 0x1.6f460ap-1f, 0x1.6f45fap-1f}, 0x1.ef310cp-1f}, //     8  
  {{ 0x1.6fc9a8p-1f, 0x1.6fc99ap-1f}, 0x1.ef410cp-1f}, //     7  
  {{ 0x1.704d54p-1f, 0x1.704d46p-1f}, 0x1.ef510cp-1f}, //     7  
  {{ 0x1.70d0fap-1f, 0x1.70d0ecp-1f}, 0x1.ef610ap-1f}, //     7  
  {{ 0x1.7154acp-1f, 0x1.71549ep-1f}, 0x1.ef7108p-1f}, //     7  
  {{ 0x1.71d87ap-1f, 0x1.71d86cp-1f}, 0x1.ef8108p-1f}, //     7  
  {{ 0x1.725c54p-1f, 0x1.725c46p-1f}, 0x1.ef9108p-1f}, //     7  
  {{ 0x1.72e028p-1f, 0x1.72e01ap-1f}, 0x1.efa106p-1f}, //     7  
  {{ 0x1.736408p-1f, 0x1.7363fap-1f}, 0x1.efb104p-1f}, //     7  
  {{ 0x1.73e806p-1f, 0x1.73e7f6p-1f}, 0x1.efc104p-1f}, //     8  
  {{ 0x1.746c0ep-1f, 0x1.746bfep-1f}, 0x1.efd104p-1f}, //     8  
  {{ 0x1.74f010p-1f, 0x1.74f002p-1f}, 0x1.efe102p-1f}, //     7  
  {{ 0x1.757420p-1f, 0x1.757410p-1f}, 0x1.eff100p-1f}, //     8  
  {{ 0x1.75f84ap-1f, 0x1.75f83cp-1f}, 0x1.f00100p-1f}, //     7  
  {{ 0x1.767c80p-1f, 0x1.767c72p-1f}, 0x1.f01100p-1f}, //     7  
  {{ 0x1.7700b2p-1f, 0x1.7700a4p-1f}, 0x1.f020fep-1f}, //     7  
  {{ 0x1.7784f0p-1f, 0x1.7784e2p-1f}, 0x1.f030fcp-1f}, //     7  
  {{ 0x1.78094ap-1f, 0x1.78093ap-1f}, 0x1.f040fcp-1f}, //     8  
  {{ 0x1.788daep-1f, 0x1.788da0p-1f}, 0x1.f050fcp-1f}, //     7  
  {{ 0x1.79120ep-1f, 0x1.791200p-1f}, 0x1.f060fap-1f}, //     7  
  {{ 0x1.79967ap-1f, 0x1.79966cp-1f}, 0x1.f070f8p-1f}, //     7  
  {{ 0x1.7a1b02p-1f, 0x1.7a1af4p-1f}, 0x1.f080f8p-1f}, //     7  
  {{ 0x1.7a9f96p-1f, 0x1.7a9f86p-1f}, 0x1.f090f8p-1f}, //     8  
  {{ 0x1.7b2424p-1f, 0x1.7b2416p-1f}, 0x1.f0a0f6p-1f}, //     7  
  {{ 0x1.7ba8bep-1f, 0x1.7ba8b0p-1f}, 0x1.f0b0f4p-1f}, //     7  
  {{ 0x1.7c2d74p-1f, 0x1.7c2d66p-1f}, 0x1.f0c0f4p-1f}, //     7  
  {{ 0x1.7cb238p-1f, 0x1.7cb228p-1f}, 0x1.f0d0f4p-1f}, //     8  
  {{ 0x1.7d36f4p-1f, 0x1.7d36e6p-1f}, 0x1.f0e0f2p-1f}, //     7  
  {{ 0x1.7dbbbep-1f, 0x1.7dbbaep-1f}, 0x1.f0f0f0p-1f}, //     8  
  {{ 0x1.7e40a2p-1f, 0x1.7e4094p-1f}, 0x1.f100f0p-1f}, //     7  
  {{ 0x1.7ec592p-1f, 0x1.7ec584p-1f}, 0x1.f110f0p-1f}, //     7  
  {{ 0x1.7f4a7ep-1f, 0x1.7f4a70p-1f}, 0x1.f120eep-1f}, //     7  
  {{ 0x1.7fcf76p-1f, 0x1.7fcf68p-1f}, 0x1.f130ecp-1f}, //     7  
  {{ 0x1.80548ap-1f, 0x1.80547ap-1f}, 0x1.f140ecp-1f}, //     8  
  {{ 0x1.80d9a8p-1f, 0x1.80d99ap-1f}, 0x1.f150ecp-1f}, //     7  
  {{ 0x1.815ec4p-1f, 0x1.815eb4p-1f}, 0x1.f160eap-1f}, //     8  
  {{ 0x1.81e3eap-1f, 0x1.81e3dap-1f}, 0x1.f170e8p-1f}, //     8  
  {{ 0x1.82692cp-1f, 0x1.82691cp-1f}, 0x1.f180e8p-1f}, //     8  
  {{ 0x1.82ee7ap-1f, 0x1.82ee6ap-1f}, 0x1.f190e8p-1f}, //     8  
  {{ 0x1.8373c2p-1f, 0x1.8373b4p-1f}, 0x1.f1a0e6p-1f}, //     7  
  {{ 0x1.83f916p-1f, 0x1.83f908p-1f}, 0x1.f1b0e4p-1f}, //     7  
  {{ 0x1.847e88p-1f, 0x1.847e78p-1f}, 0x1.f1c0e4p-1f}, //     8  
  {{ 0x1.850404p-1f, 0x1.8503f6p-1f}, 0x1.f1d0e4p-1f}, //     7  
  {{ 0x1.85897cp-1f, 0x1.85896cp-1f}, 0x1.f1e0e2p-1f}, //     8  
  {{ 0x1.860efep-1f, 0x1.860ef0p-1f}, 0x1.f1f0e0p-1f}, //     7  
  {{ 0x1.86949ep-1f, 0x1.869490p-1f}, 0x1.f200e0p-1f}, //     7  
  {{ 0x1.871a4ap-1f, 0x1.871a3ap-1f}, 0x1.f210e0p-1f}, //     8  
  {{ 0x1.879ff0p-1f, 0x1.879fe2p-1f}, 0x1.f220dep-1f}, //     7  
  {{ 0x1.8825a2p-1f, 0x1.882592p-1f}, 0x1.f230dcp-1f}, //     8  
  {{ 0x1.88ab70p-1f, 0x1.88ab62p-1f}, 0x1.f240dcp-1f}, //     7  
  {{ 0x1.89314ap-1f, 0x1.89313cp-1f}, 0x1.f250dcp-1f}, //     7  
  {{ 0x1.89b71ep-1f, 0x1.89b710p-1f}, 0x1.f260dap-1f}, //     7  
  {{ 0x1.8a3d00p-1f, 0x1.8a3cf0p-1f}, 0x1.f270d8p-1f}, //     8  
  {{ 0x1.8ac2fcp-1f, 0x1.8ac2eep-1f}, 0x1.f280d8p-1f}, //     7  
  {{ 0x1.8b4904p-1f, 0x1.8b48f6p-1f}, 0x1.f290d8p-1f}, //     7  
  {{ 0x1.8bcf08p-1f, 0x1.8bcefap-1f}, 0x1.f2a0d6p-1f}, //     7  
  {{ 0x1.8c5518p-1f, 0x1.8c5508p-1f}, 0x1.f2b0d4p-1f}, //     8  
  {{ 0x1.8cdb44p-1f, 0x1.8cdb34p-1f}, 0x1.f2c0d4p-1f}, //     8  
  {{ 0x1.8d617ap-1f, 0x1.8d616cp-1f}, 0x1.f2d0d4p-1f}, //     7  
  {{ 0x1.8de7acp-1f, 0x1.8de79ep-1f}, 0x1.f2e0d2p-1f}, //     7  
  {{ 0x1.8e6deap-1f, 0x1.8e6ddcp-1f}, 0x1.f2f0d0p-1f}, //     7  
  {{ 0x1.8ef446p-1f, 0x1.8ef436p-1f}, 0x1.f300d0p-1f}, //     8  
  {{ 0x1.8f7aacp-1f, 0x1.8f7a9cp-1f}, 0x1.f310d0p-1f}, //     8  
  {{ 0x1.90010cp-1f, 0x1.9000fep-1f}, 0x1.f320cep-1f}, //     7  
  {{ 0x1.90877ap-1f, 0x1.90876ap-1f}, 0x1.f330ccp-1f}, //     8  
  {{ 0x1.910e02p-1f, 0x1.910df4p-1f}, 0x1.f340ccp-1f}, //     7  
  {{ 0x1.919498p-1f, 0x1.919488p-1f}, 0x1.f350ccp-1f}, //     8  
  {{ 0x1.921b28p-1f, 0x1.921b18p-1f}, 0x1.f360cap-1f}, //     8  
  {{ 0x1.92a1c2p-1f, 0x1.92a1b4p-1f}, 0x1.f370c8p-1f}, //     7  
  {{ 0x1.93287ap-1f, 0x1.93286cp-1f}, 0x1.f380c8p-1f}, //     7  
  {{ 0x1.93af3ep-1f, 0x1.93af30p-1f}, 0x1.f390c8p-1f}, //     7  
  {{ 0x1.9435fcp-1f, 0x1.9435eep-1f}, 0x1.f3a0c6p-1f}, //     7  
  {{ 0x1.94bcc8p-1f, 0x1.94bcb8p-1f}, 0x1.f3b0c4p-1f}, //     8  
  {{ 0x1.9543aep-1f, 0x1.9543a0p-1f}, 0x1.f3c0c4p-1f}, //     7  
  {{ 0x1.95caa0p-1f, 0x1.95ca92p-1f}, 0x1.f3d0c4p-1f}, //     7  
  {{ 0x1.96518ep-1f, 0x1.965180p-1f}, 0x1.f3e0c2p-1f}, //     7  
  {{ 0x1.96d888p-1f, 0x1.96d878p-1f}, 0x1.f3f0c0p-1f}, //     8  
  {{ 0x1.975f9cp-1f, 0x1.975f8ep-1f}, 0x1.f400c0p-1f}, //     7  
  {{ 0x1.97e6bep-1f, 0x1.97e6b0p-1f}, 0x1.f410c0p-1f}, //     7  
  {{ 0x1.986ddap-1f, 0x1.986dccp-1f}, 0x1.f420bep-1f}, //     7  
  {{ 0x1.98f502p-1f, 0x1.98f4f4p-1f}, 0x1.f430bcp-1f}, //     7  
  {{ 0x1.997c46p-1f, 0x1.997c38p-1f}, 0x1.f440bcp-1f}, //     7  
  {{ 0x1.9a0398p-1f, 0x1.9a0388p-1f}, 0x1.f450bcp-1f}, //     8  
  {{ 0x1.9a8ae2p-1f, 0x1.9a8ad4p-1f}, 0x1.f460bap-1f}, //     7  
  {{ 0x1.9b123ap-1f, 0x1.9b122ap-1f}, 0x1.f470b8p-1f}, //     8  
  {{ 0x1.9b99acp-1f, 0x1.9b999ep-1f}, 0x1.f480b8p-1f}, //     7  
  {{ 0x1.9c212cp-1f, 0x1.9c211cp-1f}, 0x1.f490b8p-1f}, //     8  
  {{ 0x1.9ca8a6p-1f, 0x1.9ca896p-1f}, 0x1.f4a0b6p-1f}, //     8  
  {{ 0x1.9d302cp-1f, 0x1.9d301cp-1f}, 0x1.f4b0b4p-1f}, //     8  
  {{ 0x1.9db7cep-1f, 0x1.9db7bep-1f}, 0x1.f4c0b4p-1f}, //     8  
  {{ 0x1.9e3f7cp-1f, 0x1.9e3f6cp-1f}, 0x1.f4d0b4p-1f}, //     8  
  {{ 0x1.9ec724p-1f, 0x1.9ec716p-1f}, 0x1.f4e0b2p-1f}, //     7  
  {{ 0x1.9f4edap-1f, 0x1.9f4ecap-1f}, 0x1.f4f0b0p-1f}, //     8  
  {{ 0x1.9fd6aap-1f, 0x1.9fd69cp-1f}, 0x1.f500b0p-1f}, //     7  
  {{ 0x1.a05e88p-1f, 0x1.a05e78p-1f}, 0x1.f510b0p-1f}, //     8  
  {{ 0x1.a0e660p-1f, 0x1.a0e650p-1f}, 0x1.f520aep-1f}, //     8  
  {{ 0x1.a16e44p-1f, 0x1.a16e34p-1f}, 0x1.f530acp-1f}, //     8  
  {{ 0x1.a1f644p-1f, 0x1.a1f634p-1f}, 0x1.f540acp-1f}, //     8  
  {{ 0x1.a27e50p-1f, 0x1.a27e40p-1f}, 0x1.f550acp-1f}, //     8  
  {{ 0x1.a30656p-1f, 0x1.a30648p-1f}, 0x1.f560aap-1f}, //     7  
  {{ 0x1.a38e68p-1f, 0x1.a38e5ap-1f}, 0x1.f570a8p-1f}, //     7  
  {{ 0x1.a41698p-1f, 0x1.a4168ap-1f}, 0x1.f580a8p-1f}, //     7  
  {{ 0x1.a49ed4p-1f, 0x1.a49ec4p-1f}, 0x1.f590a8p-1f}, //     8  
  {{ 0x1.a52708p-1f, 0x1.a526fap-1f}, 0x1.f5a0a6p-1f}, //     7  
  {{ 0x1.a5af4ap-1f, 0x1.a5af3cp-1f}, 0x1.f5b0a4p-1f}, //     7  
  {{ 0x1.a637a8p-1f, 0x1.a6379ap-1f}, 0x1.f5c0a4p-1f}, //     7  
  {{ 0x1.a6c012p-1f, 0x1.a6c004p-1f}, 0x1.f5d0a4p-1f}, //     7  
  {{ 0x1.a74878p-1f, 0x1.a74868p-1f}, 0x1.f5e0a2p-1f}, //     8  
  {{ 0x1.a7d0e8p-1f, 0x1.a7d0d8p-1f}, 0x1.f5f0a0p-1f}, //     8  
  {{ 0x1.a85976p-1f, 0x1.a85966p-1f}, 0x1.f600a0p-1f}, //     8  
  {{ 0x1.a8e20ep-1f, 0x1.a8e200p-1f}, 0x1.f610a0p-1f}, //     7  
  {{ 0x1.a96aa2p-1f, 0x1.a96a94p-1f}, 0x1.f6209ep-1f}, //     7  
  {{ 0x1.a9f342p-1f, 0x1.a9f332p-1f}, 0x1.f6309cp-1f}, //     8  
  {{ 0x1.aa7bfep-1f, 0x1.aa7beep-1f}, 0x1.f6409cp-1f}, //     8  
  {{ 0x1.ab04c6p-1f, 0x1.ab04b8p-1f}, 0x1.f6509cp-1f}, //     7  
  {{ 0x1.ab8d8ap-1f, 0x1.ab8d7ap-1f}, 0x1.f6609ap-1f}, //     8  
  {{ 0x1.ac1658p-1f, 0x1.ac1648p-1f}, 0x1.f67098p-1f}, //     8  
  {{ 0x1.ac9f44p-1f, 0x1.ac9f34p-1f}, 0x1.f68098p-1f}, //     8  
  {{ 0x1.ad283ap-1f, 0x1.ad282cp-1f}, 0x1.f69098p-1f}, //     7  
  {{ 0x1.adb12cp-1f, 0x1.adb11ep-1f}, 0x1.f6a096p-1f}, //     7  
  {{ 0x1.ae3a2ap-1f, 0x1.ae3a1ap-1f}, 0x1.f6b094p-1f}, //     8  
  {{ 0x1.aec344p-1f, 0x1.aec336p-1f}, 0x1.f6c094p-1f}, //     7  
  {{ 0x1.af4c6ap-1f, 0x1.af4c5cp-1f}, 0x1.f6d094p-1f}, //     7  
  {{ 0x1.afd58cp-1f, 0x1.afd57cp-1f}, 0x1.f6e092p-1f}, //     8  
  {{ 0x1.b05eb8p-1f, 0x1.b05eaap-1f}, 0x1.f6f090p-1f}, //     7  
  {{ 0x1.b0e802p-1f, 0x1.b0e7f4p-1f}, 0x1.f70090p-1f}, //     7  
  {{ 0x1.b17158p-1f, 0x1.b17148p-1f}, 0x1.f71090p-1f}, //     8  
  {{ 0x1.b1faa8p-1f, 0x1.b1fa98p-1f}, 0x1.f7208ep-1f}, //     8  
  {{ 0x1.b28404p-1f, 0x1.b283f4p-1f}, 0x1.f7308cp-1f}, //     8  
  {{ 0x1.b30d7cp-1f, 0x1.b30d6ep-1f}, 0x1.f7408cp-1f}, //     7  
  {{ 0x1.b39702p-1f, 0x1.b396f2p-1f}, 0x1.f7508cp-1f}, //     8  
  {{ 0x1.b42080p-1f, 0x1.b42072p-1f}, 0x1.f7608ap-1f}, //     7  
  {{ 0x1.b4aa0cp-1f, 0x1.b4a9fcp-1f}, 0x1.f77088p-1f}, //     8  
  {{ 0x1.b533b4p-1f, 0x1.b533a4p-1f}, 0x1.f78088p-1f}, //     8  
  {{ 0x1.b5bd68p-1f, 0x1.b5bd58p-1f}, 0x1.f79088p-1f}, //     8  
  {{ 0x1.b64716p-1f, 0x1.b64706p-1f}, 0x1.f7a086p-1f}, //     8  
  {{ 0x1.b6d0d0p-1f, 0x1.b6d0c2p-1f}, 0x1.f7b084p-1f}, //     7  
  {{ 0x1.b75aa8p-1f, 0x1.b75a98p-1f}, 0x1.f7c084p-1f}, //     8  
  {{ 0x1.b7e48ap-1f, 0x1.b7e47cp-1f}, 0x1.f7d084p-1f}, //     7  
  {{ 0x1.b86e68p-1f, 0x1.b86e58p-1f}, 0x1.f7e082p-1f}, //     8  
  {{ 0x1.b8f852p-1f, 0x1.b8f842p-1f}, 0x1.f7f080p-1f}, //     8  
  {{ 0x1.b98258p-1f, 0x1.b98248p-1f}, 0x1.f80080p-1f}, //     8  
  {{ 0x1.ba0c6ap-1f, 0x1.ba0c5cp-1f}, 0x1.f81080p-1f}, //     7  
  {{ 0x1.ba9678p-1f, 0x1.ba9668p-1f}, 0x1.f8207ep-1f}, //     8  
  {{ 0x1.bb2090p-1f, 0x1.bb2080p-1f}, 0x1.f8307cp-1f}, //     8  
  {{ 0x1.bbaac6p-1f, 0x1.bbaab6p-1f}, 0x1.f8407cp-1f}, //     8  
  {{ 0x1.bc3506p-1f, 0x1.bc34f8p-1f}, 0x1.f8507cp-1f}, //     7  
  {{ 0x1.bcbf44p-1f, 0x1.bcbf34p-1f}, 0x1.f8607ap-1f}, //     8  
  {{ 0x1.bd498ap-1f, 0x1.bd497cp-1f}, 0x1.f87078p-1f}, //     7  
  {{ 0x1.bdd3f0p-1f, 0x1.bdd3e0p-1f}, 0x1.f88078p-1f}, //     8  
  {{ 0x1.be5e60p-1f, 0x1.be5e52p-1f}, 0x1.f89078p-1f}, //     7  
  {{ 0x1.bee8ccp-1f, 0x1.bee8bcp-1f}, 0x1.f8a076p-1f}, //     8  
  {{ 0x1.bf7344p-1f, 0x1.bf7334p-1f}, 0x1.f8b074p-1f}, //     8  
  {{ 0x1.bffdd8p-1f, 0x1.bffdc8p-1f}, 0x1.f8c074p-1f}, //     8  
  {{ 0x1.c08878p-1f, 0x1.c08868p-1f}, 0x1.f8d074p-1f}, //     8  
  {{ 0x1.c11312p-1f, 0x1.c11302p-1f}, 0x1.f8e072p-1f}, //     8  
  {{ 0x1.c19db8p-1f, 0x1.c19daap-1f}, 0x1.f8f070p-1f}, //     7  
  {{ 0x1.c2287cp-1f, 0x1.c2286cp-1f}, 0x1.f90070p-1f}, //     8  
  {{ 0x1.c2b34cp-1f, 0x1.c2b33cp-1f}, 0x1.f91070p-1f}, //     8  
  {{ 0x1.c33e16p-1f, 0x1.c33e06p-1f}, 0x1.f9206ep-1f}, //     8  
  {{ 0x1.c3c8ecp-1f, 0x1.c3c8dcp-1f}, 0x1.f9306cp-1f}, //     8  
  {{ 0x1.c453dep-1f, 0x1.c453cep-1f}, 0x1.f9406cp-1f}, //     8  
  {{ 0x1.c4dedcp-1f, 0x1.c4decep-1f}, 0x1.f9506cp-1f}, //     7  
  {{ 0x1.c569d6p-1f, 0x1.c569c6p-1f}, 0x1.f9606ap-1f}, //     8  
  {{ 0x1.c5f4dcp-1f, 0x1.c5f4ccp-1f}, 0x1.f97068p-1f}, //     8  
  {{ 0x1.c67ffep-1f, 0x1.c67feep-1f}, 0x1.f98068p-1f}, //     8  
  {{ 0x1.c70b2cp-1f, 0x1.c70b1cp-1f}, 0x1.f99068p-1f}, //     8  
  {{ 0x1.c79654p-1f, 0x1.c79644p-1f}, 0x1.f9a066p-1f}, //     8  
  {{ 0x1.c82188p-1f, 0x1.c8217ap-1f}, 0x1.f9b064p-1f}, //     7  
  {{ 0x1.c8acdap-1f, 0x1.c8acccp-1f}, 0x1.f9c064p-1f}, //     7  
  {{ 0x1.c93838p-1f, 0x1.c93828p-1f}, 0x1.f9d064p-1f}, //     8  
  {{ 0x1.c9c390p-1f, 0x1.c9c380p-1f}, 0x1.f9e062p-1f}, //     8  
  {{ 0x1.ca4ef4p-1f, 0x1.ca4ee4p-1f}, 0x1.f9f060p-1f}, //     8  
  {{ 0x1.cada74p-1f, 0x1.cada66p-1f}, 0x1.fa0060p-1f}, //     7  
  {{ 0x1.cb6602p-1f, 0x1.cb65f2p-1f}, 0x1.fa1060p-1f}, //     8  
  {{ 0x1.cbf18ap-1f, 0x1.cbf17ap-1f}, 0x1.fa205ep-1f}, //     8  
  {{ 0x1.cc7d1cp-1f, 0x1.cc7d0cp-1f}, 0x1.fa305cp-1f}, //     8  
  {{ 0x1.cd08ccp-1f, 0x1.cd08bep-1f}, 0x1.fa405cp-1f}, //     7  
  {{ 0x1.cd948ap-1f, 0x1.cd947ap-1f}, 0x1.fa505cp-1f}, //     8  
  {{ 0x1.ce2040p-1f, 0x1.ce2030p-1f}, 0x1.fa605ap-1f}, //     8  
  {{ 0x1.ceac02p-1f, 0x1.ceabf4p-1f}, 0x1.fa7058p-1f}, //     7  
  {{ 0x1.cf37e2p-1f, 0x1.cf37d4p-1f}, 0x1.fa8058p-1f}, //     7  
  {{ 0x1.cfc3cep-1f, 0x1.cfc3c0p-1f}, 0x1.fa9058p-1f}, //     7  
  {{ 0x1.d04fb4p-1f, 0x1.d04fa6p-1f}, 0x1.faa056p-1f}, //     7  
  {{ 0x1.d0dba8p-1f, 0x1.d0db98p-1f}, 0x1.fab054p-1f}, //     8  
  {{ 0x1.d167b6p-1f, 0x1.d167a6p-1f}, 0x1.fac054p-1f}, //     8  
  {{ 0x1.d1f3d2p-1f, 0x1.d1f3c2p-1f}, 0x1.fad054p-1f}, //     8  
  {{ 0x1.d27fe8p-1f, 0x1.d27fd8p-1f}, 0x1.fae052p-1f}, //     8  
  {{ 0x1.d30c0ap-1f, 0x1.d30bfap-1f}, 0x1.faf050p-1f}, //     8  
  {{ 0x1.d39848p-1f, 0x1.d39838p-1f}, 0x1.fb0050p-1f}, //     8  
  {{ 0x1.d42492p-1f, 0x1.d42484p-1f}, 0x1.fb1050p-1f}, //     7  
  {{ 0x1.d4b0d8p-1f, 0x1.d4b0c8p-1f}, 0x1.fb204ep-1f}, //     8  
  {{ 0x1.d53d2ap-1f, 0x1.d53d1ap-1f}, 0x1.fb304cp-1f}, //     8  
  {{ 0x1.d5c998p-1f, 0x1.d5c988p-1f}, 0x1.fb404cp-1f}, //     8  
  {{ 0x1.d65612p-1f, 0x1.d65602p-1f}, 0x1.fb504cp-1f}, //     8  
  {{ 0x1.d6e286p-1f, 0x1.d6e278p-1f}, 0x1.fb604ap-1f}, //     7  
  {{ 0x1.d76f08p-1f, 0x1.d76ef8p-1f}, 0x1.fb7048p-1f}, //     8  
  {{ 0x1.d7fba6p-1f, 0x1.d7fb96p-1f}, 0x1.fb8048p-1f}, //     8  
  {{ 0x1.d88850p-1f, 0x1.d88840p-1f}, 0x1.fb9048p-1f}, //     8  
  {{ 0x1.d914f4p-1f, 0x1.d914e4p-1f}, 0x1.fba046p-1f}, //     8  
  {{ 0x1.d9a1a4p-1f, 0x1.d9a194p-1f}, 0x1.fbb044p-1f}, //     8  
  {{ 0x1.da2e72p-1f, 0x1.da2e62p-1f}, 0x1.fbc044p-1f}, //     8  
  {{ 0x1.dabb4cp-1f, 0x1.dabb3cp-1f}, 0x1.fbd044p-1f}, //     8  
  {{ 0x1.db481ep-1f, 0x1.db4810p-1f}, 0x1.fbe042p-1f}, //     7  
  {{ 0x1.dbd4fep-1f, 0x1.dbd4f0p-1f}, 0x1.fbf040p-1f}, //     7  
  {{ 0x1.dc61fcp-1f, 0x1.dc61ecp-1f}, 0x1.fc0040p-1f}, //     8  
  {{ 0x1.dcef06p-1f, 0x1.dceef6p-1f}, 0x1.fc1040p-1f}, //     8  
  {{ 0x1.dd7c08p-1f, 0x1.dd7bf8p-1f}, 0x1.fc203ep-1f}, //     8  
  {{ 0x1.de0918p-1f, 0x1.de0908p-1f}, 0x1.fc303cp-1f}, //     8  
  {{ 0x1.de9644p-1f, 0x1.de9634p-1f}, 0x1.fc403cp-1f}, //     8  
  {{ 0x1.df237ep-1f, 0x1.df236ep-1f}, 0x1.fc503cp-1f}, //     8  
  {{ 0x1.dfb0b0p-1f, 0x1.dfb0a0p-1f}, 0x1.fc603ap-1f}, //     8  
  {{ 0x1.e03df0p-1f, 0x1.e03de0p-1f}, 0x1.fc7038p-1f}, //     8  
  {{ 0x1.e0cb4cp-1f, 0x1.e0cb3cp-1f}, 0x1.fc8038p-1f}, //     8  
  {{ 0x1.e158b4p-1f, 0x1.e158a4p-1f}, 0x1.fc9038p-1f}, //     8  
  {{ 0x1.e1e616p-1f, 0x1.e1e608p-1f}, 0x1.fca036p-1f}, //     7  
  {{ 0x1.e27386p-1f, 0x1.e27376p-1f}, 0x1.fcb034p-1f}, //     8  
  {{ 0x1.e30112p-1f, 0x1.e30102p-1f}, 0x1.fcc034p-1f}, //     8  
  {{ 0x1.e38eaap-1f, 0x1.e38e9ap-1f}, 0x1.fcd034p-1f}, //     8  
  {{ 0x1.e41c3cp-1f, 0x1.e41c2cp-1f}, 0x1.fce032p-1f}, //     8  
  {{ 0x1.e4a9dap-1f, 0x1.e4a9cap-1f}, 0x1.fcf030p-1f}, //     8  
  {{ 0x1.e53796p-1f, 0x1.e53786p-1f}, 0x1.fd0030p-1f}, //     8  
  {{ 0x1.e5c55ep-1f, 0x1.e5c54ep-1f}, 0x1.fd1030p-1f}, //     8  
  {{ 0x1.e65320p-1f, 0x1.e65310p-1f}, 0x1.fd202ep-1f}, //     8  
  {{ 0x1.e6e0eep-1f, 0x1.e6e0dep-1f}, 0x1.fd302cp-1f}, //     8  
  {{ 0x1.e76edap-1f, 0x1.e76ecap-1f}, 0x1.fd402cp-1f}, //     8  
  {{ 0x1.e7fcd0p-1f, 0x1.e7fcc2p-1f}, 0x1.fd502cp-1f}, //     7  
  {{ 0x1.e88ac2p-1f, 0x1.e88ab2p-1f}, 0x1.fd602ap-1f}, //     8  
  {{ 0x1.e918c0p-1f, 0x1.e918b0p-1f}, 0x1.fd7028p-1f}, //     8  
  {{ 0x1.e9a6dcp-1f, 0x1.e9a6ccp-1f}, 0x1.fd8028p-1f}, //     8  
  {{ 0x1.ea3502p-1f, 0x1.ea34f4p-1f}, 0x1.fd9028p-1f}, //     7  
  {{ 0x1.eac324p-1f, 0x1.eac314p-1f}, 0x1.fda026p-1f}, //     8  
  {{ 0x1.eb5152p-1f, 0x1.eb5142p-1f}, 0x1.fdb024p-1f}, //     8  
  {{ 0x1.ebdf9cp-1f, 0x1.ebdf8cp-1f}, 0x1.fdc024p-1f}, //     8  
  {{ 0x1.ec6df4p-1f, 0x1.ec6de4p-1f}, 0x1.fdd024p-1f}, //     8  
  {{ 0x1.ecfc44p-1f, 0x1.ecfc36p-1f}, 0x1.fde022p-1f}, //     7  
  {{ 0x1.ed8aa2p-1f, 0x1.ed8a92p-1f}, 0x1.fdf020p-1f}, //     8  
  {{ 0x1.ee191cp-1f, 0x1.ee190ep-1f}, 0x1.fe0020p-1f}, //     7  
  {{ 0x1.eea7a4p-1f, 0x1.eea794p-1f}, 0x1.fe1020p-1f}, //     8  
  {{ 0x1.ef3624p-1f, 0x1.ef3614p-1f}, 0x1.fe201ep-1f}, //     8  
  {{ 0x1.efc4b2p-1f, 0x1.efc4a2p-1f}, 0x1.fe301cp-1f}, //     8  
  {{ 0x1.f0535cp-1f, 0x1.f0534cp-1f}, 0x1.fe401cp-1f}, //     8  
  {{ 0x1.f0e212p-1f, 0x1.f0e204p-1f}, 0x1.fe501cp-1f}, //     7  
  {{ 0x1.f170c4p-1f, 0x1.f170b4p-1f}, 0x1.fe601ap-1f}, //     8  
  {{ 0x1.f1ff80p-1f, 0x1.f1ff70p-1f}, 0x1.fe7018p-1f}, //     8  
  {{ 0x1.f28e5ap-1f, 0x1.f28e4cp-1f}, 0x1.fe8018p-1f}, //     7  
  {{ 0x1.f31d42p-1f, 0x1.f31d32p-1f}, 0x1.fe9018p-1f}, //     8  
  {{ 0x1.f3ac22p-1f, 0x1.f3ac12p-1f}, 0x1.fea016p-1f}, //     8  
  {{ 0x1.f43b0ep-1f, 0x1.f43afep-1f}, 0x1.feb014p-1f}, //     8  
  {{ 0x1.f4ca18p-1f, 0x1.f4ca08p-1f}, 0x1.fec014p-1f}, //     8  
  {{ 0x1.f55930p-1f, 0x1.f55920p-1f}, 0x1.fed014p-1f}, //     8  
  {{ 0x1.f5e840p-1f, 0x1.f5e830p-1f}, 0x1.fee012p-1f}, //     8  
  {{ 0x1.f6775cp-1f, 0x1.f6774cp-1f}, 0x1.fef010p-1f}, //     8  
  {{ 0x1.f70696p-1f, 0x1.f70686p-1f}, 0x1.ff0010p-1f}, //     8  
  {{ 0x1.f795dcp-1f, 0x1.f795ccp-1f}, 0x1.ff1010p-1f}, //     8  
  {{ 0x1.f8251cp-1f, 0x1.f8250cp-1f}, 0x1.ff200ep-1f}, //     8  
  {{ 0x1.f8b468p-1f, 0x1.f8b45ap-1f}, 0x1.ff300cp-1f}, //     7  
  {{ 0x1.f943d4p-1f, 0x1.f943c4p-1f}, 0x1.ff400cp-1f}, //     8  
  {{ 0x1.f9d34ap-1f, 0x1.f9d33ap-1f}, 0x1.ff500cp-1f}, //     8  
  {{ 0x1.fa62bap-1f, 0x1.fa62aap-1f}, 0x1.ff600ap-1f}, //     8  
  {{ 0x1.faf236p-1f, 0x1.faf226p-1f}, 0x1.ff7008p-1f}, //     8  
  {{ 0x1.fb81d0p-1f, 0x1.fb81c0p-1f}, 0x1.ff8008p-1f}, //     8  
  {{ 0x1.fc1176p-1f, 0x1.fc1166p-1f}, 0x1.ff9008p-1f}, //     8  
  {{ 0x1.fca116p-1f, 0x1.fca106p-1f}, 0x1.ffa006p-1f}, //     8  
  {{ 0x1.fd30c2p-1f, 0x1.fd30b2p-1f}, 0x1.ffb004p-1f}, //     8  
  {{ 0x1.fdc08cp-1f, 0x1.fdc07cp-1f}, 0x1.ffc004p-1f}, //     8  
  {{ 0x1.fe5062p-1f, 0x1.fe5052p-1f}, 0x1.ffd004p-1f}, //     8  
  {{ 0x1.fee032p-1f, 0x1.fee022p-1f}, 0x1.ffe002p-1f}, //     8  
  {{ 0x1.ff700ep-1f, 0x1.ff6ffep-1f}, 0x1.fff000p-1f}, //     8  
};
