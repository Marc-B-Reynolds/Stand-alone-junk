// Public Domain under http://unlicense.org, see link for details.
//
// *****EXCEPT:************************
// 1) reference (cr_acoshf) version:
// ************************************
//
// The CORE-MATH routine fall under:
//  
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "internal/f32_math_common.h"
#include "internal/f32_asincos.h"

#include "util.h"

//**********************************************************************

float libm(float x) { return acoshf(x); }


//**********************************************************************
// SEE: https://core-math.gitlabpages.inria.fr
// and license info at top of file.

// oh my!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshorten-64-to-32"
#pragma GCC diagnostic ignored "-Wimplicit-float-conversion"
#pragma GCC diagnostic ignored "-Wimplicit-int-conversion"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

/* Argument reduction for log: for 0 <= i < 256, we multiply the range
   [1+i/256,1+(i+1)/256] by T[i] to get a centered interval around 1.
   An exception is i=0, for which we use T[0]=1, to avoid rounding errors
   for the exact case log(1)=0. */
static const double T[256] = {
  1, 0x1.fd04794a10e6ap-1, 0x1.fb0c610d5e939p-1,
  0x1.f9182b6813bafp-1, 0x1.f727cce5f530ap-1, 0x1.f53b3a3fa204ep-1,
  0x1.f3526859b8cecp-1, 0x1.f16d4c4401f17p-1, 0x1.ef8bdb389ebadp-1,
  0x1.edae0a9b3d3a5p-1, 0x1.ebd3cff850b0cp-1, 0x1.e9fd21044e799p-1,
  0x1.e829f39aef509p-1, 0x1.e65a3dbe74d6bp-1, 0x1.e48df596f3394p-1,
  0x1.e2c511719ee16p-1, 0x1.e0ff87c01e1p-1, 0x1.df3d4f17de4dbp-1,
  0x1.dd7e5e316d94cp-1, 0x1.dbc2abe7d71d4p-1, 0x1.da0a2f3803b41p-1,
  0x1.d854df401d855p-1, 0x1.d6a2b33ef7448p-1, 0x1.d4f3a293769cap-1,
  0x1.d347a4bc01d34p-1, 0x1.d19eb155f08a4p-1, 0x1.cff8c01cff8cp-1,
  0x1.ce55c8eac79p-1, 0x1.ccb5c3b636e3ap-1, 0x1.cb18a8930de6p-1,
  0x1.c97e6fb15e44dp-1, 0x1.c7e7115d0ce95p-1, 0x1.c65285fd56843p-1,
  0x1.c4c0c61456a8ep-1, 0x1.c331ca3e91679p-1, 0x1.c1a58b327f576p-1,
  0x1.c01c01c01c01cp-1, 0x1.be9526d0769fap-1, 0x1.bd10f365451b6p-1,
  0x1.bb8f609879493p-1, 0x1.ba10679bd8488p-1, 0x1.b89401b89401cp-1,
  0x1.b71a284ee6b34p-1, 0x1.b5a2d4d5b081fp-1, 0x1.b42e00da17007p-1,
  0x1.b2bba5ff26a23p-1, 0x1.b14bbdfd760e6p-1, 0x1.afde42a2cb482p-1,
  0x1.ae732dd1c2a09p-1, 0x1.ad0a798177693p-1, 0x1.aba41fbd2e5b1p-1,
  0x1.aa401aa401aa4p-1, 0x1.a8de64688ebabp-1, 0x1.a77ef750a56dap-1,
  0x1.a621cdb4f8fdfp-1, 0x1.a4c6e200d2637p-1, 0x1.a36e2eb1c432dp-1,
  0x1.a217ae575ff2fp-1, 0x1.a0c35b92ecdf1p-1, 0x1.9f713117200dp-1,
  0x1.9e2129a7d5f0ap-1, 0x1.9cd34019cd34p-1, 0x1.9b876f5262dd1p-1,
  0x1.9a3db2474fb98p-1, 0x1.98f603fe670ap-1, 0x1.97b05f8d56652p-1,
  0x1.966cc01966ccp-1, 0x1.952b20d73ee97p-1, 0x1.93eb7d0aa6759p-1,
  0x1.92add0064ab74p-1, 0x1.9172152b841ddp-1, 0x1.903847ea1cec1p-1,
  0x1.8f0063c018fp-1, 0x1.8dca64397e408p-1, 0x1.8c9644f01efbcp-1,
  0x1.8b64018b64019p-1, 0x1.8a3395c018a34p-1, 0x1.8904fd503744bp-1,
  0x1.87d8340ab6e97p-1, 0x1.86ad35cb59a84p-1, 0x1.8583fe7a7c018p-1,
  0x1.845c8a0ce5129p-1, 0x1.8336d48397a24p-1, 0x1.8212d9eba4018p-1,
  0x1.80f0965dfabcbp-1, 0x1.7fd005ff4018p-1, 0x1.7eb124ffa053bp-1,
  0x1.7d93ef9aa4b46p-1, 0x1.7c7862170949fp-1, 0x1.7b5e78c693733p-1,
  0x1.7a463005e918cp-1, 0x1.792f843c689c3p-1, 0x1.781a71dc01782p-1,
  0x1.7706f5610d8dp-1, 0x1.75f50b522b17cp-1, 0x1.74e4b040174e5p-1,
  0x1.73d5e0c5899f7p-1, 0x1.72c899870f91fp-1, 0x1.71bcd732e940ap-1,
  0x1.70b29680e66fap-1, 0x1.6fa9d4324438p-1, 0x1.6ea28d118b474p-1,
  0x1.6d9cbdf26eaefp-1, 0x1.6c9863b1ab429p-1, 0x1.6b957b34e7803p-1,
  0x1.6a94016a94017p-1, 0x1.6993f349cc726p-1, 0x1.68954dd2390bap-1,
  0x1.67980e0bf08c7p-1, 0x1.669c31075ab4p-1, 0x1.65a1b3dd13357p-1,
  0x1.64a893adcd25fp-1, 0x1.63b0cda236e1cp-1, 0x1.62ba5eeade65ep-1,
  0x1.61c544c0161c5p-1, 0x1.60d17c61da198p-1, 0x1.5fdf0317b5c6fp-1,
  0x1.5eedd630a9fb3p-1, 0x1.5dfdf303137b6p-1, 0x1.5d0f56ec91e57p-1,
  0x1.5c21ff51ef005p-1, 0x1.5b35e99f06714p-1, 0x1.5a4b1346add2bp-1,
  0x1.596179c29d2cep-1, 0x1.58791a9357ccep-1, 0x1.5791f34015792p-1,
  0x1.56ac0156ac015p-1, 0x1.55c7426b79286p-1, 0x1.54e3b4194ce66p-1,
  0x1.5401540154015p-1, 0x1.53201fcb02fb1p-1, 0x1.5240152401524p-1,
  0x1.516131c015161p-1, 0x1.508373590ec9cp-1, 0x1.4fa6d7aeb597cp-1,
  0x1.4ecb5c86b3d24p-1, 0x1.4df0ffac83c01p-1, 0x1.4d17bef15cb4ep-1,
  0x1.4c3f982c20723p-1, 0x1.4b68893948d1cp-1, 0x1.4a928ffad5b5cp-1,
  0x1.49bdaa583b401p-1, 0x1.48e9d63e504d1p-1, 0x1.4817119f3d325p-1,
  0x1.47455a726abf2p-1, 0x1.4674aeb4717e9p-1, 0x1.45a50c670938fp-1,
  0x1.44d67190f8b43p-1, 0x1.4408dc3e05b22p-1, 0x1.433c4a7ee52b4p-1,
  0x1.4270ba692bc4dp-1, 0x1.41a62a173e821p-1, 0x1.40dc97a843ae8p-1,
  0x1.4014014014014p-1, 0x1.3f4c65072bf74p-1, 0x1.3e85c12a9d651p-1,
  0x1.3dc013dc013dcp-1, 0x1.3cfb5b51698ebp-1, 0x1.3c3795c553afbp-1,
  0x1.3b74c1769aa5cp-1, 0x1.3ab2dca869b81p-1, 0x1.39f1e5a22f36ep-1,
  0x1.3931daaf8f721p-1, 0x1.3872ba2057e04p-1, 0x1.37b4824872744p-1,
  0x1.36f7317fd9212p-1, 0x1.363ac622898b1p-1, 0x1.357f3e9078e5bp-1,
  0x1.34c4992d87fd9p-1, 0x1.340ad461776d3p-1, 0x1.3351ee97dbfc6p-1,
  0x1.3299e6401329ap-1, 0x1.31e2b9cd37dc2p-1, 0x1.312c67b6173eep-1,
  0x1.3076ee7525c2cp-1, 0x1.2fc24c8874486p-1, 0x1.2f0e8071a5703p-1,
  0x1.2e5b88b5e3104p-1, 0x1.2da963ddd3cfbp-1, 0x1.2cf8107590e67p-1,
  0x1.2c478d0c9c013p-1, 0x1.2b97d835d548ep-1, 0x1.2ae8f087718dp-1,
  0x1.2a3ad49af0907p-1, 0x1.298d830d1378p-1, 0x1.28e0fa7dd35a3p-1,
  0x1.2835399057efdp-1, 0x1.278a3eeaee65p-1, 0x1.26e009370049cp-1,
  0x1.263697210aa18p-1, 0x1.258de75895121p-1, 0x1.24e5f89029305p-1,
  0x1.243ec97d49eaep-1, 0x1.239858d86b11fp-1, 0x1.22f2a55ce8fc5p-1,
  0x1.224dadc900489p-1, 0x1.21a970ddc5ba7p-1, 0x1.2105ed5f1e336p-1,
  0x1.20632213b6c6dp-1, 0x1.1fc10dc4fce8bp-1, 0x1.1f1faf3f16b64p-1,
  0x1.1e7f0550db594p-1, 0x1.1ddf0ecbcb841p-1, 0x1.1d3fca840a074p-1,
  0x1.1ca13750547fep-1, 0x1.1c035409fc1dfp-1, 0x1.1b661f8cde833p-1,
  0x1.1ac998b75eb9p-1, 0x1.1a2dbe6a5e3e4p-1, 0x1.19928f89362b7p-1,
  0x1.18f80af9b06dcp-1, 0x1.185e2fa401186p-1, 0x1.17c4fc72bfcb9p-1,
  0x1.172c7052e1316p-1, 0x1.16948a33b08fap-1, 0x1.15fd4906c96f1p-1,
  0x1.1566abc011567p-1, 0x1.14d0b155b19aep-1, 0x1.143b58c01143bp-1,
  0x1.13a6a0f9cf01ep-1, 0x1.131288ffbb3b6p-1, 0x1.127f0fd0d2295p-1,
  0x1.11ec346e36092p-1, 0x1.1159f5db29606p-1, 0x1.10c8531d0952ep-1,
  0x1.10374b3b480aap-1, 0x1.0fa6dd3f67322p-1, 0x1.0f170834f27fap-1,
  0x1.0e87cb297a51ep-1, 0x1.0df9252c8e5e6p-1, 0x1.0d6b154fb86f9p-1,
  0x1.0cdd9aa677344p-1, 0x1.0c50b446391f3p-1, 0x1.0bc4614657569p-1,
  0x1.0b38a0c010b39p-1, 0x1.0aad71ce84d16p-1, 0x1.0a22d38eaf2bfp-1,
  0x1.0998c51f624d5p-1, 0x1.090f45a1430aap-1, 0x1.08865436c3cf7p-1,
  0x1.07fdf0041ff7cp-1, 0x1.0776182f57386p-1, 0x1.06eecbe029155p-1,
  0x1.06680a4010668p-1, 0x1.05e1d27a3ee9cp-1, 0x1.055c23bb98e2ap-1,
  0x1.04d6fd32b0c7bp-1, 0x1.04525e0fc2fcbp-1, 0x1.03ce4584b19ap-1,
  0x1.034ab2c50040dp-1, 0x1.02c7a505cffbfp-1, 0x1.02451b7ddb2d2p-1,
  0x1.01c315657186bp-1, 0x1.014191f674111p-1, 0x1.00c0906c513cfp-1,
  0x1.0040100401004p-1,
};

/* U[i] approximates -log(T[i]) */
static const double U[256] = {
  0, 0x1.7ee11ebd82ec4p-8, 0x1.3e7295d25a7d5p-7,
  0x1.bcf712c743853p-7, 0x1.1d7f7eb9eebf1p-6, 0x1.5c45a51b8d393p-6,
  0x1.9ace7551cc515p-6, 0x1.d91a66c543cbep-6, 0x1.0b94f7c196173p-5,
  0x1.2a7ec2214e879p-5, 0x1.494acc34d911dp-5, 0x1.67f94f094bd92p-5,
  0x1.868a83083f6dp-5, 0x1.a4fe9ffa3d233p-5, 0x1.c355dd0921f2fp-5,
  0x1.e19070c27601p-5, 0x1.ffae9119b92fbp-5, 0x1.0ed839b5526fep-4,
  0x1.1dcb263db1944p-4, 0x1.2cb0283f5de22p-4, 0x1.3b87598b1b6fp-4,
  0x1.4a50d3aa1b03fp-4, 0x1.590cafdf01c26p-4, 0x1.67bb0726ec0fbp-4,
  0x1.765bf23a6be17p-4, 0x1.84ef898e82828p-4, 0x1.9375e55595edfp-4,
  0x1.a1ef1d8061cd8p-4, 0x1.b05b49bee4403p-4, 0x1.beba818146764p-4,
  0x1.cd0cdbf8c13ep-4,
  /* the following entry for i=32 was modified manually from
     0x1.db5270187d925p-4 to avoid an exceptional case for x=0x1.1ff606p+32f */
  0x1.db5270187d972p-4,
  0x1.e98b54967146bp-4,
  0x1.f7b79fec37de2p-4, 0x1.02ebb42bf3d4ap-3, 0x1.09f561ee719c4p-3,
  0x1.10f8e422539b1p-3, 0x1.17f6458fca611p-3, 0x1.1eed90e2dc2c3p-3,
  0x1.25ded0abc6ad3p-3, 0x1.2cca0f5f5f252p-3, 0x1.33af575770e4dp-3,
  0x1.3a8eb2d31a375p-3, 0x1.41682bf727bbfp-3, 0x1.483bccce6e3dcp-3,
  0x1.4f099f4a230b1p-3, 0x1.55d1ad4232d7p-3,
  /* the following entry for i=47 was modified manually from
     0x1.5c940075972b9p-3 to avoid an exceptional case for x=0x1.2fe614p+116f */
  0x1.5c940075973e8p-3,
  0x1.6350a28aaa759p-3, 0x1.6a079d0f7aadp-3, 0x1.70b8f97a1aa74p-3,
  0x1.7764c128f2127p-3, 0x1.7e0afd630c276p-3, 0x1.84abb75865137p-3,
  0x1.8b46f8223625bp-3, 0x1.91dcc8c340bdfp-3, 0x1.986d3228180c8p-3,
  0x1.9ef83d2769a34p-3, 0x1.a57df28244dcbp-3, 0x1.abfe5ae46124ap-3,
  0x1.b2797ee46320cp-3, 0x1.b8ef670420c3bp-3, 0x1.bf601bb0e44ep-3,
  0x1.c5cba543ae424p-3, 0x1.cc320c0176501p-3, 0x1.d293581b6b3e7p-3,
  0x1.d8ef91af31d5ep-3, 0x1.df46c0c722d3p-3, 0x1.e598ed5a87e2ep-3,
  0x1.ebe61f4dd7b0bp-3, 0x1.f22e5e72f105cp-3, 0x1.f871b28955045p-3,
  0x1.feb0233e607cep-3, 0x1.0274dc16c232fp-2, 0x1.058f3c703ebc5p-2,
  0x1.08a73667c57aep-2, 0x1.0bbccdb0d24bcp-2, 0x1.0ed005f657da5p-2,
  0x1.11e0e2dad9cb6p-2, 0x1.14ef67f88685ap-2, 0x1.17fb98e15095ep-2,
  0x1.1b05791f07b4ap-2, 0x1.1e0d0c33716bdp-2, 0x1.211255986160cp-2,
  0x1.241558bfd1405p-2, 0x1.27161913f853dp-2, 0x1.2a1499f762bcap-2,
  0x1.2d10dec508582p-2, 0x1.300aead06350cp-2, 0x1.3302c1658658ap-2,
  0x1.35f865c93293ep-2, 0x1.38ebdb38ed32p-2, 0x1.3bdd24eb14b69p-2,
  0x1.3ecc460ef5f5p-2, 0x1.41b941cce0beep-2, 0x1.44a41b463c47bp-2,
  0x1.478cd5959b3d8p-2, 0x1.4a7373cecf997p-2, 0x1.4d57f8fefe27fp-2,
  /* the following entry for i=99 was modified manually from
     0x1.503a682cb1cb3p-2 to avoid an exceptional case for x=0x1.6351d8p+94f */
  0x1.503a682cb1d49p-2,
  0x1.531ac457ee77fp-2, 0x1.55f9107a43ee2p-2,
  0x1.58d54f86e02f3p-2, 0x1.5baf846aa1b1ap-2, 0x1.5e87b20c2954ap-2,
  0x1.615ddb4bec13cp-2, 0x1.64320304447c1p-2, 0x1.67042c0983e3p-2,
  0x1.69d4592a0362ep-2, 0x1.6ca28d2e34986p-2, 0x1.6f6ecad8b2292p-2,
  0x1.723914e6500e2p-2, 0x1.75016e0e2ba63p-2, 0x1.77c7d901bb913p-2,
  0x1.7a8c586cdf545p-2, 0x1.7d4eeef5eec6ep-2, 0x1.800f9f3dc94ccp-2,
  0x1.82ce6bdfe4d9ep-2, 0x1.858b57725cc43p-2, 0x1.8846648600623p-2,
  0x1.8aff95a661781p-2, 0x1.8db6ed59e272dp-2, 0x1.906c6e21c4753p-2,
  0x1.93201a7a35336p-2, 0x1.95d1f4da5ca0ap-2, 0x1.9881ffb46a6fp-2,
  0x1.9b303d75a362p-2, 0x1.9ddcb0866e742p-2, 0x1.a0875b4a61d17p-2,
  0x1.a33040204fa64p-2, 0x1.a5d7616252c36p-2, 0x1.a87cc165db199p-2,
  0x1.ab20627bba0ap-2, 0x1.adc246f02e9p-2, 0x1.b062710af141cp-2,
  0x1.b300e30f402a2p-2, 0x1.b59d9f3bea7c3p-2, 0x1.b838a7cb5c1efp-2,
  0x1.bad1fef3a9167p-2, 0x1.bd69a6e698c46p-2, 0x1.bfffa1d1b1084p-2,
  0x1.c293f1de4137dp-2, 0x1.c52699316cf6cp-2, 0x1.c7b799ec36eafp-2,
  0x1.ca46f62b8b4e6p-2, 0x1.ccd4b0084a5efp-2, 0x1.cf60c99752ad9p-2,
  0x1.d1eb44e98b4c9p-2, 0x1.d474240beddd7p-2, 0x1.d6fb6907907eap-2,
  0x1.d98115e1af9b6p-2, 0x1.dc052c9bb79abp-2, 0x1.de87af334e71cp-2,
  0x1.e1089fa25d168p-2, 0x1.e387ffdf18d77p-2, 0x1.e605d1dc0c931p-2,
  0x1.e882178821d52p-2, 0x1.eafcd2cea9d72p-2, 0x1.ed7605976663dp-2,
  0x1.efedb1c692a07p-2, 0x1.f263d93cebbb9p-2, 0x1.f4d87dd7b97e6p-2,
  0x1.f74ba170d6c7ep-2, 0x1.f9bd45deb9ea5p-2, 0x1.fc2d6cf47cf1cp-2,
  0x1.fe9c1881e5cfep-2, 0x1.0084a529b7386p-1, 0x1.01ba8219265a4p-1,
  0x1.02efa3f23d29cp-1, 0x1.04240b965e54cp-1, 0x1.0557b9e55634ep-1,
  0x1.068aafbd5e9dap-1, 0x1.07bcedfb229fep-1,
  /* the following entry for i=173 was modified manually from
     0x1.08ee7579c2413p-1 to avoid an exceptional case for x=0x1.ad3d1p+57f */
  0x1.08ee7579c2425p-1,
  0x1.0a1f4712d6292p-1, 0x1.0b4f639e73429p-1, 0x1.0c7ecbf32e532p-1,
  /* the following entry for i=177 was modified manually from
     0x1.0dad80e61f87ap-1 to avoid an exceptional case for x=0x1.b121a6p+75f */
  0x1.0dad80e61f8a9p-1,
  0x1.0edb834ae5f5ep-1, 0x1.1008d3f3ab146p-1,
  0x1.113573b126281p-1, 0x1.126163529fa7ap-1, 0x1.138ca3a5f494fp-1,
  0x1.14b7357799cd2p-1, 0x1.15e119929f4e4p-1, 0x1.170a50c0b3749p-1,
  0x1.1832dbca262d9p-1, 0x1.195abb75ec21ap-1, 0x1.1a81f089a1d56p-1,
  0x1.1ba87bc98ec1ap-1, 0x1.1cce5df8a8622p-1, 0x1.1df397d8953bfp-1,
  0x1.1f182a29afdb1p-1, 0x1.203c15ab09c7ap-1, 0x1.215f5b1a6e729p-1,
  0x1.2281fb34661ap-1, 0x1.23a3f6b438a52p-1, 0x1.24c54e53f0793p-1,
  0x1.25e602cc5d448p-1, 0x1.270614d516c38p-1, 0x1.282585247f7d3p-1,
  0x1.2944546fc777ap-1, 0x1.2a62836aeee59p-1, 0x1.2b8012c8c8ccp-1,
  0x1.2c9d033afda0fp-1, 0x1.2db955720de23p-1, 0x1.2ed50a1d54a5ap-1,
  0x1.2ff021eb0a221p-1, 0x1.310a9d8846313p-1,
  /* the following entry for i=209 was modified manually from
     0x1.32247da102ca6p-1 to avoid an exceptional case for x=0x1.d1309cp+61f */
  0x1.32247da102ca5p-1,
  0x1.333dc2e01e776p-1, 0x1.34566def5ec14p-1, 0x1.356e7f7772978p-1,
  0x1.3685f81ff4b06p-1, 0x1.379cd88f6de2bp-1, 0x1.38b3216b5778dp-1,
  0x1.39c8d3581d7ecp-1, 0x1.3addeef92108p-1, 0x1.3bf274f0ba70dp-1,
  0x1.3d0665e03b98fp-1, 0x1.3e19c267f2182p-1, 0x1.3f2c8b27296cdp-1,
  0x1.403ec0bc2d255p-1, 0x1.415063c44b02cp-1, 0x1.426174dbd5166p-1,
  0x1.4371f49e23d9dp-1, 0x1.4481e3a59840ep-1, 0x1.4591428b9dc68p-1,
  0x1.46a011e8ac746p-1, 0x1.47ae52544ae45p-1, 0x1.48bc0465103d8p-1,
  0x1.49c928b0a62bep-1, 0x1.4ad5bfcbcad23p-1, 0x1.4be1ca4a52b77p-1,
  0x1.4ced48bf2aaf3p-1, 0x1.4df83bbc59bc9p-1, 0x1.4f02a3d302f06p-1,
  0x1.500c819367434p-1, 0x1.5115d58ce769bp-1, 0x1.521ea04e05a45p-1,
  0x1.5326e264678adp-1, 0x1.542e9c5cd7d2ep-1, 0x1.5535cec348128p-1,
  0x1.563c7a22d27ccp-1, 0x1.57429f05bb9b9p-1, 0x1.58483df574045p-1,
  0x1.594d577a9a07fp-1, 0x1.5a51ec1cfb5f4p-1, 0x1.5b55fc6396d2bp-1,
  0x1.5c5988d49dddep-1, 0x1.5d5c91f5764f1p-1, 0x1.5e5f184abbe28p-1,
  0x1.5f611c5841d9fp-1, 0x1.60629ea1148fep-1, 0x1.61639fa77b069p-1,
  0x1.62641fecf8743p-1,
};

typedef union { double x; uint64_t n; } union64_t;

#define BIAS 1023

/* assuming x >= 1, return an approximation of log(x) */
static double
fast_log (double x)
{
  union64_t u;
  u.x = x;
  uint64_t n = u.n;
  uint64_t e = (n >> 52) - BIAS;
  n = n & 0xffffffffffffful;
  u.n = ((uint64_t) BIAS << 52) | n;
  double t = u.x;
  /* now x equals t*2^e with 1 <= t < 2, thus log(x) = e*log(2) + log(t) */
  int i = n >> 44; /* 0 <= i < 256 */
  t = t * T[i] - 1.0;
  /* use a degree-5 approximation for log(1+t) from log_wide.sollya */
  static const double P[5] = { 1, -0x1.fffffffffcb15p-2, 0x1.55555555518dbp-2,
    -0x1.000035201fd82p-2, 0x1.9999fdc78d70fp-3 };
  double y = P[4];
  y = P[3] + y * t;
  y = P[2] + y * t;
  y = P[1] + y * t;
  y = P[0] + y * t;
  static const double log2 = 0x1.62e42fefa39efp-1;
  return (double) e * log2 + (U[i] + y * t);
}

float
cr_acoshf (float x)
{
  /* deal here with NaN, +Inf and -Inf */

  if (x <= 0x1p0f){ /* acosh(x) is NaN for x < 1 */
    if(x == 0x1p0f) return 0.0f;
    return sqrtf (-0x1p0f);
  }

  /* use Equation (17.1.2) from [1]: acosh(x) = log (x + sqrt(x^2-1)) */
  double y;

  /* for x large, we have acosh(x) ~ log(2*x) */
  double xx = x;
  if (x > 0x1p21f)
  {
    y = 2.0 * xx;
    return fast_log (y);
  }

  y = xx - 1.0;
  y = xx + sqrt (y * (xx + 1.0));
  return fast_log (y);
}

#pragma GCC diagnostic pop

//********************************************************


func_entry_t func_table[] =
{
  ENTRY(libm),
};

const char* func_name = "acosh";

float cr_func(float x) { return cr_acoshf(x); }

#include "common.h"

//********************************************************

void test_simple()
{
}

void test_all()
{
  uint32_t x0 = f32_to_bits(0.0f);
  uint32_t x1 = f32_to_bits(1.0f);
  test_force(x0,x1);
}

int main(void)
{
#if 0
  //test_lo();
  //test_hi();
  test_hi();
  test_1pot(0.25f);
  test_1pot(0.125f);
  //test_1pot(0x1.0p-100f);
  //test_di();
#else  
  test_all();
#endif  

  error_dump();

  return 0;
}



