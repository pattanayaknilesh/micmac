// File Automatically generated by eLiSe
#include "StdAfx.h"
#include "cEqBBCamSecond.h"


cEqBBCamSecond::cEqBBCamSecond():
    cElCompiledFonc(2)
{
   AddIntRef (cIncIntervale("C2",3,4));
   AddIntRef (cIncIntervale("D2",4,5));
   AddIntRef (cIncIntervale("Omega2",0,3));
   AddIntRef (cIncIntervale("Tmp_PTer",5,8));
   Close(false);
}



void cEqBBCamSecond::ComputeVal()
{
   double tmp0_ = mCompCoord[7];
   double tmp1_ = mCompCoord[6];
   double tmp2_ = mCompCoord[1];
   double tmp3_ = mCompCoord[5];
   double tmp4_ = mCompCoord[3];
   double tmp5_ = mCompCoord[4];
   double tmp6_ = mCompCoord[2];
   double tmp7_ = mCompCoord[0];
   double tmp8_ = tmp7_*tmp1_;
   double tmp9_ = tmp2_*tmp3_;
   double tmp10_ = tmp8_-tmp9_;
   double tmp11_ = tmp0_+tmp10_;
   double tmp12_ = (tmp11_)-mLocVecB2_z;
   double tmp13_ = mLocVecC2_z*tmp4_;
   double tmp14_ = tmp12_-tmp13_;
   double tmp15_ = mLocVecD2_z*tmp5_;
   double tmp16_ = tmp14_-tmp15_;

  mVal[0] = mLocI2_x-((tmp3_+tmp2_*tmp0_-tmp6_*tmp1_)-mLocVecB2_x-mLocVecC2_x*tmp4_-mLocVecD2_x*tmp5_)/(tmp16_);

  mVal[1] = mLocI2_y-((tmp1_+tmp6_*tmp3_-tmp7_*tmp0_)-mLocVecB2_y-mLocVecC2_y*tmp4_-mLocVecD2_y*tmp5_)/(tmp16_);

}


void cEqBBCamSecond::ComputeValDeriv()
{
   double tmp0_ = mCompCoord[7];
   double tmp1_ = mCompCoord[6];
   double tmp2_ = mCompCoord[1];
   double tmp3_ = mCompCoord[5];
   double tmp4_ = mCompCoord[3];
   double tmp5_ = mCompCoord[4];
   double tmp6_ = tmp2_*tmp0_;
   double tmp7_ = mCompCoord[2];
   double tmp8_ = tmp7_*tmp1_;
   double tmp9_ = tmp6_-tmp8_;
   double tmp10_ = tmp3_+tmp9_;
   double tmp11_ = (tmp10_)-mLocVecB2_x;
   double tmp12_ = mLocVecC2_x*tmp4_;
   double tmp13_ = tmp11_-tmp12_;
   double tmp14_ = mLocVecD2_x*tmp5_;
   double tmp15_ = tmp13_-tmp14_;
   double tmp16_ = mCompCoord[0];
   double tmp17_ = tmp16_*tmp1_;
   double tmp18_ = tmp2_*tmp3_;
   double tmp19_ = tmp17_-tmp18_;
   double tmp20_ = tmp0_+tmp19_;
   double tmp21_ = (tmp20_)-mLocVecB2_z;
   double tmp22_ = mLocVecC2_z*tmp4_;
   double tmp23_ = tmp21_-tmp22_;
   double tmp24_ = mLocVecD2_z*tmp5_;
   double tmp25_ = tmp23_-tmp24_;
   double tmp26_ = ElSquare(tmp25_);
   double tmp27_ = tmp7_*tmp3_;
   double tmp28_ = tmp16_*tmp0_;
   double tmp29_ = tmp27_-tmp28_;
   double tmp30_ = tmp1_+tmp29_;
   double tmp31_ = (tmp30_)-mLocVecB2_y;
   double tmp32_ = mLocVecC2_y*tmp4_;
   double tmp33_ = tmp31_-tmp32_;
   double tmp34_ = mLocVecD2_y*tmp5_;
   double tmp35_ = tmp33_-tmp34_;
   double tmp36_ = -(tmp3_);
   double tmp37_ = -(mLocVecC2_z);
   double tmp38_ = -(mLocVecD2_z);
   double tmp39_ = -(tmp2_);

  mVal[0] = mLocI2_x-(tmp15_)/(tmp25_);

  mCompDer[0][0] = -(-((tmp15_)*tmp1_)/tmp26_);
  mCompDer[0][1] = -((tmp0_*(tmp25_)-(tmp15_)*tmp36_)/tmp26_);
  mCompDer[0][2] = -((-(tmp1_)*(tmp25_))/tmp26_);
  mCompDer[0][3] = -((-(mLocVecC2_x)*(tmp25_)-(tmp15_)*tmp37_)/tmp26_);
  mCompDer[0][4] = -((-(mLocVecD2_x)*(tmp25_)-(tmp15_)*tmp38_)/tmp26_);
  mCompDer[0][5] = -((tmp25_-(tmp15_)*tmp39_)/tmp26_);
  mCompDer[0][6] = -((-(tmp7_)*(tmp25_)-(tmp15_)*tmp16_)/tmp26_);
  mCompDer[0][7] = -((tmp2_*(tmp25_)-(tmp15_))/tmp26_);
  mVal[1] = mLocI2_y-(tmp35_)/(tmp25_);

  mCompDer[1][0] = -((-(tmp0_)*(tmp25_)-(tmp35_)*tmp1_)/tmp26_);
  mCompDer[1][1] = -(-((tmp35_)*tmp36_)/tmp26_);
  mCompDer[1][2] = -((tmp3_*(tmp25_))/tmp26_);
  mCompDer[1][3] = -((-(mLocVecC2_y)*(tmp25_)-(tmp35_)*tmp37_)/tmp26_);
  mCompDer[1][4] = -((-(mLocVecD2_y)*(tmp25_)-(tmp35_)*tmp38_)/tmp26_);
  mCompDer[1][5] = -((tmp7_*(tmp25_)-(tmp35_)*tmp39_)/tmp26_);
  mCompDer[1][6] = -((tmp25_-(tmp35_)*tmp16_)/tmp26_);
  mCompDer[1][7] = -((-(tmp16_)*(tmp25_)-(tmp35_))/tmp26_);
}


void cEqBBCamSecond::ComputeValDerivHessian()
{
  ELISE_ASSERT(false,"Foncteur cEqBBCamSecond Has no Der Sec");
}

void cEqBBCamSecond::SetI2_x(double aVal){ mLocI2_x = aVal;}
void cEqBBCamSecond::SetI2_y(double aVal){ mLocI2_y = aVal;}
void cEqBBCamSecond::SetVecB2_x(double aVal){ mLocVecB2_x = aVal;}
void cEqBBCamSecond::SetVecB2_y(double aVal){ mLocVecB2_y = aVal;}
void cEqBBCamSecond::SetVecB2_z(double aVal){ mLocVecB2_z = aVal;}
void cEqBBCamSecond::SetVecC2_x(double aVal){ mLocVecC2_x = aVal;}
void cEqBBCamSecond::SetVecC2_y(double aVal){ mLocVecC2_y = aVal;}
void cEqBBCamSecond::SetVecC2_z(double aVal){ mLocVecC2_z = aVal;}
void cEqBBCamSecond::SetVecD2_x(double aVal){ mLocVecD2_x = aVal;}
void cEqBBCamSecond::SetVecD2_y(double aVal){ mLocVecD2_y = aVal;}
void cEqBBCamSecond::SetVecD2_z(double aVal){ mLocVecD2_z = aVal;}



double * cEqBBCamSecond::AdrVarLocFromString(const std::string & aName)
{
   if (aName == "I2_x") return & mLocI2_x;
   if (aName == "I2_y") return & mLocI2_y;
   if (aName == "VecB2_x") return & mLocVecB2_x;
   if (aName == "VecB2_y") return & mLocVecB2_y;
   if (aName == "VecB2_z") return & mLocVecB2_z;
   if (aName == "VecC2_x") return & mLocVecC2_x;
   if (aName == "VecC2_y") return & mLocVecC2_y;
   if (aName == "VecC2_z") return & mLocVecC2_z;
   if (aName == "VecD2_x") return & mLocVecD2_x;
   if (aName == "VecD2_y") return & mLocVecD2_y;
   if (aName == "VecD2_z") return & mLocVecD2_z;
   return 0;
}


cElCompiledFonc::cAutoAddEntry cEqBBCamSecond::mTheAuto("cEqBBCamSecond",cEqBBCamSecond::Alloc);


cElCompiledFonc *  cEqBBCamSecond::Alloc()
{  return new cEqBBCamSecond();
}

