/*Header-MicMac-eLiSe-25/06/2007

    MicMac : Multi Image Correspondances par Methodes Automatiques de Correlation
    eLiSe  : ELements of an Image Software Environnement

    www.micmac.ign.fr

   
    Copyright : Institut Geographique National
    Author : Marc Pierrot Deseilligny
    Contributors : Gregoire Maillet, Didier Boldo.

[1] M. Pierrot-Deseilligny, N. Paparoditis.
    "A multiresolution and optimization-based image matching approach:
    An application to surface reconstruction from SPOT5-HRS stereo imagery."
    In IAPRS vol XXXVI-1/W41 in ISPRS Workshop On Topographic Mapping From Space
    (With Special Emphasis on Small Satellites), Ankara, Turquie, 02-2006.

[2] M. Pierrot-Deseilligny, "MicMac, un lociel de mise en correspondance
    d'images, adapte au contexte geograhique" to appears in 
    Bulletin d'information de l'Institut Geographique National, 2007.

Francais :

   MicMac est un logiciel de mise en correspondance d'image adapte 
   au contexte de recherche en information geographique. Il s'appuie sur
   la bibliotheque de manipulation d'image eLiSe. Il est distibue sous la
   licences Cecill-B.  Voir en bas de fichier et  http://www.cecill.info.


English :

    MicMac is an open source software specialized in image matching
    for research in geographic information. MicMac is built on the
    eLiSe image library. MicMac is governed by the  "Cecill-B licence".
    See below and http://www.cecill.info.

Header-MicMac-eLiSe-25/06/2007*/

#include "Apero.h"


double  cPolynBGC3M2D_Formelle::mNbPixOfEpAngle = 1.0;
double  cPolynBGC3M2D_Formelle::mEpsGrad = 5.0;

// cPolynBGC3M2D_Formelle

/***************************************************************/
/*                                                             */
/*               cOneEq_PBGC3M2DF                              */
/*            cPolynBGC3M2D_Formelle                           */
/*                                                             */
/***************************************************************/

               // ==============================================
               //             cOneEq_PBGC3M2DF
               // ==============================================

cOneEq_PBGC3M2DF::cOneEq_PBGC3M2DF(cPolynBGC3M2D_Formelle & aPF,std::vector<double > & aCoef) :
   cElemEqFormelle (aPF.Set(),false),
   mPF     (&aPF),
   mCamCur (&(aPF.mCamCur))
{
    for (int aK=0 ; aK<int(aCoef.size()) ; aK++)
    {
        mVFCoef.push_back(aPF.Set().Alloc().NewF(&(aCoef[aK])));
    }

    CloseEEF();
    // aPF.Set().AddObj2Kill(this); => COR DUMP

}


Fonc_Num  cOneEq_PBGC3M2DF::EqFormProjCor(Pt2d<Fonc_Num> aP)
{
   //  ELISE_ASSERT(false,"cOneEq_PBGC3M2DF::FormProjCor 2 complete");
   Fonc_Num aRes = 0;
   for (int aK=0 ; aK<int(mVFCoef.size()) ; aK++)
   {
       aRes = aRes + mVFCoef[aK] * PowI(aP.x,mCamCur->DegX(aK)) *  PowI(aP.y,mCamCur->DegY(aK));
   }

   return aRes;
}


   //==========================================
   //      cCellPolBGC3M2DForm
   //==========================================



cCellPolBGC3M2DForm::cCellPolBGC3M2DForm(Pt2dr aPt,cPolynBGC3M2D_Formelle * aPF,int aDim) : 
    mPF        (aPF),
    mPtIm      (aPt),
    mActive    (aPF->CamSsCorr()->CaptHasDataGeom(mPtIm)),
    mHasDep    (true),
    mDim       (aDim),
    mValDep    (aDim)
{
   if (mActive)
   {
       aPF->CamSsCorr()->GetCenterAndPTerOnBundle(mCenter,mPTer,mPtIm);
       mNorm = vunit(mPTer-mCenter);

       for (int aK=0 ; aK<3 ; aK++)
       {
           bool Ok1,Ok2;
           Pt2dr aN1 = ProjOfTurnMatr(Ok1,aPF->mEpsRot[aK]);
           Pt2dr aN2 = ProjOfTurnMatr(Ok2,aPF->mEpsRot[aK].transpose());
           if (Ok1 && Ok2)
           {
              mValDep[aK] = aN1-aN2;
           }
           else
           {
               mActive=false;
               mHasDep = false;
           }
       }
   } 
}

Pt2dr  cCellPolBGC3M2DForm::ProjOfTurnMatr(bool & Ok, const ElMatrix<double> & aMat)
{
     Pt3dr aPTer = mCenter + aMat * (mPTer-mCenter);
     Ok =  mPF->CamSsCorr()->PIsVisibleInImage(aPTer);
     if (!Ok) return Pt2dr(0,0);
     return  mPF->CamSsCorr()->Ter2Capteur(aPTer);
}

      
cCellPolBGC3M2DForm::cCellPolBGC3M2DForm() :
    mActive   (false),
    mHasDep   (false)
{
}


       // ============ cPolynBGC3M2D_Formelle =================



cPolynBGC3M2D_Formelle::cPolynBGC3M2D_Formelle
(
        cSetEqFormelles &   aSet,
        cPolynomial_BGC3M2D aCam0,
        bool                GenCodeAppui,
        bool                GenCodeAttach,
        bool                GenCodeRot
) :
   cGenPDVFormelle (aSet),
   mCamSsCorr      (aCam0.CamSsCor()),
   mCamInit        (aCam0),
   mCamCur         (aCam0),
   mEqP3I          ((GenCodeAppui | GenCodeRot) ?  mSet.Pt3dIncTmp() : 0),
   mFAmplAppui     ("AmplApp"),
   mFAmplFixVal    ("AmplFixV"),
   mFAmplAttRot    ("AmplAttR"),
   mFCentrAppui    ("CentrApp"),
   mFCentrFixVal   ("CentrFixV"),
   mFCentrAttRot   ("CentrAttR"),
   mFP3DInit       ("PTerInit"),
   mFProjInit      ("ProjInit"),
   mFGradX         ("GradX"),
   mFGradY         ("GradY"),
   mFGradZ         ("GradZ"),
   mObsPix         ("PIm"),
   mPtFixVal       ("PFixV"),
   mFixedVal       ("FixedV"),
   mRotPt          ("RotPt"),
   mDepR1          ("DepR1"),
   mDepR2          ("DepR2"),
   mDepR3          ("DepR3"),
   mCompX          (*this,mCamCur.Cx()),
   mCompY          (*this,mCamCur.Cy()),
   mNameType       ("cGen2DBundleEgProj_Deg"+ToString(mCamCur.DegreMax())),
   mNameAttach     ("cGen2DBundleAttach_Deg"+ToString(mCamCur.DegreMax())),
   mNameRot        ("cGen2DBundleAtRot_Deg"+ToString(mCamCur.DegreMax())),
   mFoncEqResidu   (0),
   mFoncEqAttach   (0),
   mIndCenter      (-1,-1),
   mRotL2W         (ElRotation3D::Id),
   mMatW2Loc       (3,3),
   mDimMvt         (3)
{
    AllowUnsortedVarIn_SetMappingCur = true;
    mCompX.IncInterv().SetName("CX");
    mCompY.IncInterv().SetName("CY");
    if (mEqP3I)
    {
       mLIntervResiduApp.AddInterv(mEqP3I->IncInterv());
       mLIntervRot.AddInterv(mEqP3I->IncInterv());
    }
    mLIntervResiduApp.AddInterv(mCompX.IncInterv());
    mLIntervResiduApp.AddInterv(mCompY.IncInterv());
    mLIntervAttach.AddInterv(mCompX.IncInterv());
    mLIntervAttach.AddInterv(mCompY.IncInterv());

    mLIntervRot.AddInterv(mCompX.IncInterv());
    mLIntervRot.AddInterv(mCompY.IncInterv());

    if (GenCodeAppui)
    {
        GenerateCode(EqFormProj()-mObsPix.PtF(),mNameType,mLIntervResiduApp);
        return;
    } 
    if (GenCodeAttach)
    {
        GenerateCode(EqFixedVal(),mNameAttach,mLIntervAttach);
        return;
    } 
    if (GenCodeRot)
    {
        GenerateCode(EqAttachRot(),mNameRot,mLIntervRot);
        return;
    } 




    // ============================================================
    //   Calcul des cellules 
    // ============================================================


    int aNbPtsStd = 20;
    int aNbPtsMin = 1+ 2 * mCamInit.DegreMax();
    double aSzMax = 1000;

    Pt2dr aSzIm = Pt2dr(mCamCur.SzBasicCapt3D());
    double aSurfTot = aSzIm.x * aSzIm.y ;
    double aSurfCell = aSurfTot / ElSquare(aNbPtsStd);
    double aSzCell = ElMin(sqrt(aSurfCell),aSzMax);
    mNbCellX = ElMax(aNbPtsMin,round_up(aSzIm.x/aSzCell));
    mNbCellY = ElMax(aNbPtsMin,round_up(aSzIm.y/aSzCell));

    Pt2dr aCenter = aSzIm /2.0;
    double aDistMinCenter = 1e20;

    {

       mEspilonAngle= mCamSsCorr->ResolutionAngulaire();
       for (int aK=0 ; aK<3 ; aK++)
       {
           mEpsRot.push_back(ElMatrix<double>::Rotation3D(mEspilonAngle,aK));
       }
    }

    mVCells =  std::vector<std::vector<cCellPolBGC3M2DForm> > (mNbCellY+1);
    Pt2di aPInd(-1,-1);
    for (aPInd.y=0 ; aPInd.y<= mNbCellY ; aPInd.y++)
    {
        mVCells[aPInd.y] = std::vector<cCellPolBGC3M2DForm>  (mNbCellX+1);
        for (aPInd.x=0 ; aPInd.x<= mNbCellX ; aPInd.x++)
        {
            double aPdsX = aPInd.x/double(mNbCellX);
            double aPdsY = aPInd.y/double(mNbCellY);
            Pt2dr aP = aSzIm.mcbyc(Pt2dr(aPdsX,aPdsY));


            aP.x = ModifInTervGrad(aP.x,aSzIm.x); //  ElMin(ElMax(2*mEpsGrad,aP.x),aSzIm.x-2*mEpsGrad);
            aP.y = ModifInTervGrad(aP.y,aSzIm.y); //  ElMin(ElMax(2*mEpsGrad,aP.x),aSzIm.x-2*mEpsGrad);

            cCellPolBGC3M2DForm & aCurCell =  Cell(aPInd);
            aCurCell  = cCellPolBGC3M2DForm(aP,this,mDimMvt);
            if (aCurCell.mActive)
            {
                double aDist = euclid(aCurCell.mPtIm,aCenter);
                if (aDist<aDistMinCenter)
                {
                    aDistMinCenter = aDist;
                    mIndCenter = aPInd;
                }
            }
        }
    }

    ELISE_ASSERT(mIndCenter.x>0,"Cannot determine center in cPolynBGC3M2D_Formelle");

    Pt3dr aZ = Cell(mIndCenter).mNorm;
    Pt3dr aX = Cell(mIndCenter+Pt2di(1,0)).mNorm;
    Pt3dr aY = vunit(aZ ^ aX);
    aX = vunit(aY^aZ);

    mCenterGlob = Cell(mIndCenter).mCenter;
    mRotL2W = ElRotation3D(mCenterGlob,MatFromCol(aX,aY,aZ),true);
    mMatW2Loc = mRotL2W.Mat().transpose();

/*

*/

    // ElMatrix<double> aM2Loc = MatFromCol(aX,aY,aZ);

    std::cout << "NB " << mNbCellX << " " << mNbCellY << " " << mIndCenter << "\n";

}


/*
Pt2dr   cPolynBGC3M2D_Formelle::PtOfRot(const cCellPolBGC3M2DForm & aCel,const ElMatrix<double> & aMat)
{
    Pt2dr aPIm = aCel.mPtIm;

    Pt3dr aDir = 
}
*/



double cPolynBGC3M2D_Formelle::ModifInTervGrad(const double & aV,const double & aBorne) const
{
    return ElMin(ElMax(2*mEpsGrad,aV),aBorne-2*mEpsGrad);
}

Pt2dr  cPolynBGC3M2D_Formelle::P2dNL(const Pt2dr & aPt) const
{
    ElSeg3D aSeg = mCamSsCorr->Capteur2RayTer(aPt);
    Pt3dr aNorm =  mMatW2Loc*aSeg.TgNormee();
    return ProjStenope(aNorm);
}

Pt2dr cPolynBGC3M2D_Formelle::DepOfKnownSol(const Pt2dr & aP0,CamStenope * aCSOut)
{
    CamStenope * aCS = (CamStenope *) mCamSsCorr;
    Pt3dr aP1 =  aCS->ImEtProf2Terrain(aP0,1.0);
    Pt2dr aP2 = aCSOut->R3toF2(aP1);

    return aP2 - aP0;
}




Pt2dr cPolynBGC3M2D_Formelle::DepSimul(const Pt2dr & aP0,const ElMatrix<double> & aMat)
{
    CamStenope * aCS = (CamStenope *) mCamSsCorr;
    // ElSeg3D aSeg = mCamSsCorr->Capteur2RayTer(aP0);
    Pt3dr aP1 =  aCS->F2toDirRayonL3(aP0);
    aP1 = aMat * aP1;
    Pt2dr aP2 = aCS->L3toF2(aP1);

    return aP2 - aP0;
}

void cPolynBGC3M2D_Formelle::AddEqRotGlob(double aPds)
{
   AddEqRot(Pt2di(0,0),SzCell(),aPds);
}

void cPolynBGC3M2D_Formelle::AddEqRot(const Pt2di & aP0,const Pt2di &aP1,double aPds)
{
    PostInit();
    std::set<int> aSX;
    std::set<int> aSY;
    Pt2di aPInd;
    int aNbOk=0;
    for (aPInd.x=aP0.x ; aPInd.x<=aP1.x ; aPInd.x++)
    {
        for (aPInd.y=aP0.y ; aPInd.y<=aP1.y ; aPInd.y++)
        {
            if (CellHasGradValue(aPInd))
            {
               aSX.insert(aPInd.x);
               aSY.insert(aPInd.y);
               aNbOk++;
            }
       }
    }

    if (aSX.size() < 2) return;
    if (aSY.size() < 2) return;

    for (aPInd.x=aP0.x ; aPInd.x<=aP1.x ; aPInd.x++)
    {
        for (aPInd.y=aP0.y ; aPInd.y<=aP1.y ; aPInd.y++)
        {
            if (CellHasGradValue(aPInd))
            {
                cCellPolBGC3M2DForm & aCurCell =  Cell(aPInd);
                mRotPt.SetEtat(aCurCell.mPtIm);
                mDepR1.SetEtat(aCurCell.mValDep[0]);
                mDepR2.SetEtat(aCurCell.mValDep[1]);
                mDepR3.SetEtat(aCurCell.mValDep[2]);

                mEqP3I->InitEqP3iVal(Pt3dr(0,0,0));

                mSet.VAddEqFonctToSys(mFoncEqRot,aPds/aNbOk,false) ;
            }
       }
    }

    mBufSubRot->DoSubst();

}
/*
   mRotPt.SetEtat(aPIm);
   Pt2dr aValFix = Cur ? mCamCur.DeltaCamInit2CurIm(aPIm) : Pt2dr(0,0);
   mFixedVal.SetEtat(aValFix);

   mSet.VAddEqFonctToSys(mFoncEqAttach,aPds,false) ;
*/




void cPolynBGC3M2D_Formelle::TestRot(const Pt2di & aP0,const Pt2di &aP1,double & aSomD,double & aSomRot,ElMatrix<double> *aRotPert)
{
    aSomD=0;
    aSomRot=0;
    int aNbOk=0;
    L2SysSurResol aSys(3);

    Pt2di aPInd;
    for (aPInd.x=aP0.x ; aPInd.x<=aP1.x ; aPInd.x++)
    {
        for (aPInd.y=aP0.y ; aPInd.y<=aP1.y ; aPInd.y++)
        {
            if (CellHasGradValue(aPInd))
            {
                cCellPolBGC3M2DForm & aCell = Cell(aPInd);
                Pt2dr aPt = aCell.mPtIm;
                Pt2dr aDep = mCamCur.DeltaCamInit2CurIm(aPt);
                if (aRotPert) aDep = DepSimul(aPt,*aRotPert);

                aSomD += euclid(aDep);
                aNbOk++;
                //  SUM  P(K) *   mValDep[aK] =  aDep
                double aCoefX[3],aCoefY[3];

                for (int aK=0 ; aK<3 ; aK++)
                {
                     aCoefX[aK] = aCell.mValDep[aK].x;
                     aCoefY[aK] = aCell.mValDep[aK].y;
                }
                aSys.AddEquation(1.0,aCoefX,aDep.x);
                aSys.AddEquation(1.0,aCoefY,aDep.y);
            }
        }
    }

    bool Ok;
    Im1D_REAL8  aSol = aSys.Solve(&Ok);
    double * aDS = aSol.data();
    // std::cout << "SSSS " << aDS[0] << " "  << aDS[1] << " " << aDS[2] << "\n";

    for (aPInd.x=aP0.x ; aPInd.x<=aP1.x ; aPInd.x++)
    {
        for (aPInd.y=aP0.y ; aPInd.y<=aP1.y ; aPInd.y++)
        {
            if (CellHasGradValue(aPInd))
            {
                cCellPolBGC3M2DForm & aCell = Cell(aPInd);
                Pt2dr aPt = aCell.mPtIm;
                Pt2dr aDep = mCamCur.DeltaCamInit2CurIm(aPt);
                if (aRotPert)
                {
                   aDep = DepSimul(aPt,*aRotPert);
                }

                Pt2dr aDepR(0,0);
                for (int aK=0 ; aK<3 ; aK++)
                {
                    aDepR = aDepR + aCell.mValDep[aK] * aDS[aK];
                }

                aSomRot += euclid(aDep-aDepR);
            }
        }
    }

    aSomD /= aNbOk;
    aSomRot /= aNbOk;
}



bool cPolynBGC3M2D_Formelle::CellHasValue(const Pt2di & aP) const
{
    return (aP.x>=0) && (aP.x<=mNbCellX) && (aP.y>=0) && (aP.y<=mNbCellY) && Cell(aP).mActive;
}

bool cPolynBGC3M2D_Formelle::CellHasGradValue(const Pt2di & aP) const
{
    return CellHasValue(aP) &&  Cell(aP).mHasDep;
}


cBasicGeomCap3D *   cPolynBGC3M2D_Formelle::CamSsCorr() const 
{
   return mCamSsCorr;
}

void cPolynBGC3M2D_Formelle::PostInit()
{
    if (mFoncEqResidu!=0) return ;
// std::cout << "cPolynBGC3M2D_Formelle::PostInitcPolynBGC3M2D_Formelle::PostInit\n" ; getchar();

    mEqP3I  = mSet.Pt3dIncTmp();
    mLIntervResiduApp.AddInterv(mEqP3I->IncInterv());
    mLIntervRot.AddInterv(mEqP3I->IncInterv());

    mFoncEqResidu = cElCompiledFonc::AllocFromName(mNameType);
    mFoncEqAttach = cElCompiledFonc::AllocFromName(mNameAttach);
    mFoncEqRot = cElCompiledFonc::AllocFromName(mNameRot);


    if ((mFoncEqResidu==0) || (mFoncEqAttach==0) || (mFoncEqRot==0))
    {
       std::cout << "NAME = " << mNameType << " , " << mNameAttach << "\n";
       ELISE_ASSERT(false,"Can Get Code Comp for cCameraFormelle::cEqAppui");
    }


    mFoncEqResidu->SetMappingCur(mLIntervResiduApp,&mSet);
    mFoncEqAttach->SetMappingCur(mLIntervAttach,&mSet);
    mFoncEqRot->SetMappingCur(mLIntervRot,&mSet);


    mSet.AddFonct(mFoncEqResidu);
    mSet.AddFonct(mFoncEqAttach);
    mSet.AddFonct(mFoncEqRot);

    mFAmplAppui.InitAdr(*mFoncEqResidu);
    mFCentrAppui.InitAdr(*mFoncEqResidu);
    mFAmplFixVal.InitAdr(*mFoncEqAttach);
    mFCentrFixVal.InitAdr(*mFoncEqAttach);

    mFAmplAttRot.InitAdr(*mFoncEqRot);
    mFCentrAttRot.InitAdr(*mFoncEqRot);


    mFP3DInit.InitAdr(*mFoncEqResidu);
    mFProjInit.InitAdr(*mFoncEqResidu);
    mFGradX.InitAdr(*mFoncEqResidu);
    mFGradY.InitAdr(*mFoncEqResidu);
    mFGradZ.InitAdr(*mFoncEqResidu);
    mObsPix.InitAdr(*mFoncEqResidu);

    mPtFixVal.InitAdr(*mFoncEqAttach);
    mFixedVal.InitAdr(*mFoncEqAttach);

    mRotPt.InitAdr(*mFoncEqRot);
    mDepR1.InitAdr(*mFoncEqRot);
    mDepR2.InitAdr(*mFoncEqRot);
    mDepR3.InitAdr(*mFoncEqRot);

    mFAmplAppui.SetEtat(mCamCur.Ampl());
    mFCentrAppui.SetEtat(mCamCur.Center());
    mFAmplFixVal.SetEtat(mCamCur.Ampl());
    mFCentrFixVal.SetEtat(mCamCur.Center());
    mFAmplAttRot.SetEtat(mCamCur.Ampl());
    mFCentrAttRot.SetEtat(mCamCur.Center());

   
    mBufSubRot = new cSubstitueBlocIncTmp(*mEqP3I);
    mBufSubRot->AddInc(mLIntervRot);
    mBufSubRot->Close();

    // mSet.AddObj2Kill(this); ==> CORE DUMP
}

cBasicGeomCap3D * cPolynBGC3M2D_Formelle::GPF_NC_CurBGCap3D() { return & mCamCur; }
const cBasicGeomCap3D *  cPolynBGC3M2D_Formelle::GPF_CurBGCap3D() const { return & mCamCur; }

Pt2d<Fonc_Num>  cPolynBGC3M2D_Formelle::FormalCorrec(Pt2d<Fonc_Num> aPF,cVarEtat_PhgrF aFAmpl,cP2d_Etat_PhgrF aFCenter)
{
   Pt2d<Fonc_Num> aPPN =  (aPF-aFCenter.PtF()).div(aFAmpl.FN());
   return    Pt2d<Fonc_Num>
             (
                 mCompX.EqFormProjCor(aPPN),
                 mCompY.EqFormProjCor(aPPN)
             );
}

Pt2d<Fonc_Num>  cPolynBGC3M2D_Formelle::EqFormProj()
{
   Pt3d<Fonc_Num>  aPTerUnknown  = mEqP3I->PF();
   Pt3d<Fonc_Num>  aDeltaPTU = aPTerUnknown-mFP3DInit.PtF();
 

   // Fonc_Num aDptUX = aDeltaPTU.x;
   Pt2d<Fonc_Num>  aProj  =   mFProjInit.PtF() 
                            + mFGradX.PtF().mul(aDeltaPTU.x) 
                            + mFGradY.PtF().mul(aDeltaPTU.y) 
                            + mFGradZ.PtF().mul(aDeltaPTU.z);

   

   return    aProj + FormalCorrec(aProj,mFAmplAppui,mFCentrAppui);
}


Pt2d<Fonc_Num>  cPolynBGC3M2D_Formelle::EqFixedVal()
{
   return  FormalCorrec(mPtFixVal.PtF(),mFAmplFixVal,mFCentrFixVal) - mFixedVal.PtF();
}


Pt2d<Fonc_Num>  cPolynBGC3M2D_Formelle::EqAttachRot()
{
    Pt3d<Fonc_Num>  aTeta  = mEqP3I->PF();
    Pt2d<Fonc_Num> aDep = FormalCorrec(mRotPt.PtF(),mFAmplAttRot,mFCentrAttRot);


   return aDep - mDepR1.PtF().mul(aTeta.x) -mDepR2.PtF().mul(aTeta.y) - mDepR3.PtF().mul(aTeta.z);
}

/*
*/

void  cPolynBGC3M2D_Formelle::GenerateCode(Pt2d<Fonc_Num> aFormP,const std::string & aName,cIncListInterv & anInterv)
{
/*
    Pt2d<Fonc_Num>  aFProj =  FormProj() - mObsPix.PtF();
    std::vector<Fonc_Num> aV;
    aV.push_back(aFProj.x);
    aV.push_back(aFProj.y);
*/

    cElCompileFN::DoEverything
    (
        DIRECTORY_GENCODE_FORMEL,  // Directory ou est localise le code genere
        aName,  // donne les noms de fichier .cpp et .h ainsi que les nom de classe
        aFormP.ToTab(),  // expressions formelles 
        anInterv  // intervalle de reference
    );

}


void cPolynBGC3M2D_Formelle::AddEqAttach(Pt2dr aPIm,double aPds,bool Cur,CamStenope * aKnownSol)
{
   PostInit();
   mPtFixVal.SetEtat(aPIm);
   Pt2dr aValFix = Cur ? mCamCur.DeltaCamInit2CurIm(aPIm) : Pt2dr(0,0);
   if (aKnownSol)
   {
       aValFix = DepOfKnownSol(aPIm,aKnownSol);
   }
   mFixedVal.SetEtat(aValFix);

   mSet.VAddEqFonctToSys(mFoncEqAttach,aPds,false) ;
}

void cPolynBGC3M2D_Formelle::AddEqAttachGlob(double aPds,bool Cur,int aNbPts,CamStenope * aKnownSol)
{
    Pt2dr aSzIm = Pt2dr(mCamCur.SzBasicCapt3D());
    std::vector<Pt2dr> aVP;
    for (int aKx=0 ; aKx< aNbPts ; aKx++)
    {
        for (int aKy=0 ; aKy< aNbPts ; aKy++)
        {
             double aPdsX = (aKx+0.5)/aNbPts;
             double aPdsY = (aKy+0.5)/aNbPts;
             Pt2dr aP = aSzIm.mcbyc(Pt2dr(aPdsX,aPdsY));
             if (mCamSsCorr->CaptHasDataGeom(aP))
             {
                aVP.push_back(aP);
             }
        }
    }
    for (int aKP=0 ; aKP<int(aVP.size()) ; aKP++)
    {
         AddEqAttach(aVP[aKP],aPds/aVP.size(),Cur,aKnownSol);
    }
}



Pt2dr cPolynBGC3M2D_Formelle::AddEqAppuisInc(const Pt2dr & aPixObsIm,double aPds, cParamPtProj & aPPP,bool IsEqDroite)
{
   ELISE_ASSERT(!IsEqDroite,"cPolynBGC3M2D_Formelle::AddEqAppuisInc do not handle lines equation");
/*
   Pt3d<Fonc_Num>  aPTerUnknown  = mEqP3I->PF();
   Pt3d<Fonc_Num>  aDeltaPTU = aPTerUnknown-mFP3DInit.PtF();

CS :
     Si proj :::    P0 = aPTer   ,  PUnk = 0,0,0
     Sinon   ::     P0 = ??      ,  PUnk = PTer


   Si proj :::    mFProjInit : aPTer               mEqP3I : (0,0,0)
   Sinon   :::    mFProjInit : aPTer               mEqP3I : aPTer
    Pt3dr aPInit = 
*/



    Pt2dr aGx,aGy,aGz;

    Pt3dr aPTer = aPPP.mTer;
    Pt3dr aP3DTer = aPPP.mProjIsInit ? Pt3dr(0,0,0) : aPTer;


    Pt2dr aProjIm = mCamSsCorr->Ter2Capteur(aPTer);
    mFProjInit.SetEtat(aProjIm);
    mCamSsCorr->Diff(aGx,aGy,aGz,aProjIm,aPTer);

    mFP3DInit.SetEtat(aP3DTer);
    mFProjInit.SetEtat(aProjIm);
    mFGradX.SetEtat(aGx);
    mFGradY.SetEtat(aGy);
    mFGradZ.SetEtat(aGz);
    mObsPix.SetEtat(aPixObsIm);



    if (MPD_MM())
    {
        double aCheck = euclid(   (aP3DTer-mEqP3I->GetEqP3iVal()) /  ElMax(1e-9,(euclid(aP3DTer)+euclid(mEqP3I->GetEqP3iVal())))   );
       // Test apres correction du probleme en cas proj ou le Pinit est a 000 en CS
       if (aCheck>1e-4)
       {
          std::cout << "PTSSS = " << aP3DTer << " " << mEqP3I->GetEqP3iVal() << "\n";
          ELISE_ASSERT(aCheck<1e-4,"Chek disr in cPolynBGC3M2D_Formelle::AddEqAppuisInc");
       }
    }
    // mEqP3I->InitEqP3iVal(aPTer);

    std::vector<double> aVRes;

    if (aPds>0)
    {
       aVRes = mSet.VAddEqFonctToSys(mFoncEqResidu,aPds,false) ;
    }
    else
    {
       aVRes = mSet.VResiduSigne(mFoncEqResidu);
       
       if (1)
       {
           // Pt2dr aPRoj = 
           // Pt2dr 
           // std::cout << "VRES " << aVRes << "\n";
       }
    }


    ELISE_ASSERT(aVRes.size()==2,"cPolynBGC3M2D_Formelle::AddEqAppuisInc still un implemanted");
    return Pt2dr(aVRes[0],aVRes[1]);
}


cIncListInterv & cPolynBGC3M2D_Formelle::IntervAppuisPtsInc() 
{
     PostInit();
     return mLIntervResiduApp;
}

/***************************************************************/
/*                                                             */
/*                    cBGC3_Modif2D                            */
/*                                                             */
/***************************************************************/

cBGC3_Modif2D::cBGC3_Modif2D(cBasicGeomCap3D * aCam0,const std::string & aName,const std::string & aNameIma) :
    mCam0 (aCam0),
    mNameFileCam0 (aName),
    mNameIma (aNameIma),
    mSz (mCam0->SzBasicCapt3D())
{
}

ElSeg3D  cBGC3_Modif2D::Capteur2RayTer(const Pt2dr & aP) const
{
    return mCam0->Capteur2RayTer(CurIm2CamInit(aP));
}

Pt2dr  cBGC3_Modif2D::Ter2Capteur(const Pt3dr & aP) const
{
    return CamInit2CurIm(mCam0->Ter2Capteur(aP));
}

Pt2dr  cBGC3_Modif2D::Ter2CapteurSsCorrec(const Pt3dr & aP) const
{
    return mCam0->Ter2Capteur(aP);
}

Pt3dr  cBGC3_Modif2D::RoughCapteur2Terrain   (const Pt2dr & aP) const 
{
   return mCam0->RoughCapteur2Terrain(CurIm2CamInit(aP));
}



Pt2di  cBGC3_Modif2D::SzBasicCapt3D() const
{
    return mSz;
}

double cBGC3_Modif2D::ResolSolOfPt(const Pt3dr & aP) const 
{
   return mCam0->ResolSolOfPt(aP);
}


bool  cBGC3_Modif2D::CaptHasData(const Pt2dr &aP) const
{
    return  mCam0->CaptHasData(CurIm2CamInit(aP));
}

bool      cBGC3_Modif2D::PIsVisibleInImage(const Pt3dr & aP) const
{
    return mCam0->PIsVisibleInImage(aP);
}

bool      cBGC3_Modif2D::HasOpticalCenterOfPixel() const
{
   return mCam0->HasOpticalCenterOfPixel();
}


Pt3dr    cBGC3_Modif2D::OpticalCenterOfPixel(const Pt2dr & aP) const 
{
   return mCam0->OpticalCenterOfPixel(CurIm2CamInit(aP));
}


          // A AFFINER PLUS TARD !!!!!  Version de base du point fixe a 1 iter ...
Pt2dr   cBGC3_Modif2D::DeltaCurIm2CamInit(const Pt2dr & aP) const
{
    Pt2dr aSol = -DeltaCamInit2CurIm(aP);


    Pt2dr aTest = CamInit2CurIm(aP+aSol);
    aSol = aSol + (aP-aTest);


    return aSol;
}

cBasicGeomCap3D *   cBGC3_Modif2D::CamSsCor()
{
    return mCam0;
}

// ==========================================================

/***************************************************************/
/*                                                             */
/*                    cPolynomial_BGC3M2D                      */
/*                                                             */
/***************************************************************/



std::vector<int> cPolynomial_BGC3M2D::mDegX;
std::vector<int> cPolynomial_BGC3M2D::mDegY;

std::vector<double> cPolynomial_BGC3M2D::mPowX;
std::vector<double> cPolynomial_BGC3M2D::mPowY;

std::vector<double> & cPolynomial_BGC3M2D::Cx() {return mCx;}
std::vector<double> & cPolynomial_BGC3M2D::Cy() {return mCy;}

void cPolynomial_BGC3M2D::SetPow(const Pt2dr & aPN) const
{
     if (aPN==mCurPPow) return;
     mCurPPow = aPN;
     for (int aD=1 ; aD<= mDegreMax ; aD++)
     {
           mPowX[aD] = mPowX[aD-1] * aPN.x;
           mPowY[aD] = mPowY[aD-1] * aPN.y;
     }
}


Pt2dr cPolynomial_BGC3M2D::DeltaCamInit2CurIm(const Pt2dr & aP0) const 
{
      Pt2dr aPN = ToPNorm(aP0); 
      SetPow(aPN);
      double aSx=0 ;
      double aSy=0 ;

      for (int aK=0 ; aK<int(mCx.size()) ; aK++)
      {
          double aPXY = mPowX[mDegX[aK]] * mPowY[mDegY[aK]] ;

          aSx += mCx[aK] * aPXY;
          aSy += mCy[aK] * aPXY;
      }


      return Pt2dr(aSx,aSy);
}



cPolynomial_BGC3M2D::cPolynomial_BGC3M2D
(
      cBasicGeomCap3D * aCam0,
      const std::string & aName,
      const std::string & aNameIma,
      int aDegreeMax,
      double aRandPerturb
) :
    cBGC3_Modif2D (aCam0,aName,aNameIma),
    mDegreMax     (aDegreeMax),
    mCenter       (Pt2dr(mSz)/2.0),
    mAmpl         (euclid(mCenter)),
    mCurPPow      (0.0,0.0)
{

     int aCpt=0;
     for (int  aDegreeTot=0 ; aDegreeTot<=aDegreeMax ; aDegreeTot++)
     {
          for (int aDegX=0 ; aDegX<= aDegreeTot ; aDegX++)
          {
              int aDegY=aDegreeTot - aDegX;

              double aVx=0,aVy=0;
              if (aRandPerturb)
              {
                 aVx = aRandPerturb * NRrandC();
                 aVy = aRandPerturb * NRrandC();
              }

              mCx.push_back(aVx);
              mCy.push_back(aVy);
              if (aCpt>=int(mDegX.size()))
              {
                  mDegX.push_back(aDegX);
                  mDegY.push_back(aDegY);
              }
              aCpt++;
          }
          if (int(mPowX.size()) <= aDegreeTot)
          {
              mPowX.push_back(1.0);
              mPowY.push_back(1.0);
          }
     }
     if (0)
     {
         Show();
     }
}

void cPolynomial_BGC3M2D::Show() const
{
    std::cout << "#### DMax= " << mDegreMax 
              << "  SizPow=" << mPowX.size()
              << "\n";
    Show("CoefX",mCx);
    Show("CoefY",mCy);
}

void cPolynomial_BGC3M2D::ShowMonome(const std::string & aVar , int aDeg) const
{
    if (aDeg==0) return;
    std::cout << "*" << aVar;
    if (aDeg==1) return;
    std::cout << "^" << aDeg ;
}

cXml_PolynXY cPolynomial_BGC3M2D::ExporOneCor(const std::vector<double> & aCoeff) const
{
    cXml_PolynXY aRes;
    for (int aK=0 ; aK<int(aCoeff.size()) ; aK++)
    {
        if (aCoeff[aK])
           aRes.Monomes().push_back(cMonomXY(aCoeff[aK],mDegX[aK],mDegY[aK]));
    }
    return aRes;
}

cXml_CamGenPolBundle cPolynomial_BGC3M2D::ToXml() const
{
    cXml_CamGenPolBundle aRes;

    aRes.CorX() = ExporOneCor(mCx);
    aRes.CorY() = ExporOneCor(mCy);
    aRes.Ampl() = mAmpl;
    aRes.Center() = mCenter;
    aRes.DegreTot() = mDegreMax;
    aRes.NameIma() = mNameIma;
    aRes.NameCamSsCor() = mNameFileCam0;
    

    return aRes;
}

std::string cPolynomial_BGC3M2D::DirSave(const std::string & aDirLoc) const
{
     std::string aRes = DirOfFile(mNameIma) + "Ori-" + aDirLoc + "/";
     ELISE_fp::MkDirSvp(aRes);
     return aRes;
}

std::string cPolynomial_BGC3M2D::NameSave(const std::string & aDirLoc) const
{
    return DirSave(aDirLoc) +  "GB-Orientation-" + NameWithoutDir(mNameIma) + ".xml";
}

void cPolynomial_BGC3M2D::Save2XmlStdMMName(const std::string & aDirLoc) const
{
     std::string aDirFull = DirSave(aDirLoc);


     cXml_CamGenPolBundle aXml = ToXml();
     std::string aNameXml =  NameSave(aDirLoc);

     std::string aNameSsCor = aDirFull + NameWithoutDir(mNameFileCam0);
     if (! ELISE_fp::exist_file(aNameSsCor))
     {
            ELISE_fp::CpFile(mNameFileCam0,aNameSsCor);
     }
     aXml.NameCamSsCor() = aNameSsCor;

     MakeFileXML(aXml,aNameXml);
}


void   cPolynomial_BGC3M2D::SetMonom(const cMonomXY & aMon,std::vector<double> & aVCoef)
{
    int aNbGot=0;
    for (int aK=0 ; aK<int(aVCoef.size()) ; aK++)
    {
        if ((mDegX[aK] == aMon.mDegX) && (mDegY[aK] == aMon.mDegY))
        {
            aVCoef[aK] = aMon.mCoeff;
            aNbGot++;
        }
    }
    ELISE_ASSERT(aNbGot==1,"cPolynomial_BGC3M2D::SetMonom");
}

void   cPolynomial_BGC3M2D::SetMonom(const std::vector<cMonomXY> & aVMon,std::vector<double> & aVCoef)
{
    for (int aK=0 ; aK<int(aVMon.size()) ; aK++)
        SetMonom(aVMon[aK],aVCoef);
}
  
cPolynomial_BGC3M2D * cPolynomial_BGC3M2D::NewFromFile(const std::string & aName)
{
    cXml_CamGenPolBundle aXML =  StdGetFromSI(aName,Xml_CamGenPolBundle);

    int aType = eTIGB_Unknown;
    cBasicGeomCap3D * aCamSsCor = cBasicGeomCap3D::StdGetFromFile(aXML.NameCamSsCor(),aType);


    cPolynomial_BGC3M2D * aRes = new cPolynomial_BGC3M2D(aCamSsCor,aXML.NameCamSsCor(),aXML.NameIma(),aXML.DegreTot());

    aRes->SetMonom(aXML.CorX().Monomes(),aRes->mCx);
    aRes->SetMonom(aXML.CorY().Monomes(),aRes->mCy);

    return aRes;
    
}

cBasicGeomCap3D * cPolynomial_BGC3M2DNewFromFile (const std::string & aName)
{
  return cPolynomial_BGC3M2D::NewFromFile(aName);
}

/*
*/



void cPolynomial_BGC3M2D::Show(const std::string & aMes,const std::vector<double> & aCoef) const
{
     std::cout << " -*-*-*- " << aMes << " -*-*-*-\n";
     for (int aK=0 ; aK<int(aCoef.size()) ; aK++)
     {
          std::cout << "    ";
          std::cout << ((aK==0) ? "  " : "+ ");
          std::cout << aCoef[aK] ;
          ShowMonome("X",mDegX[aK]);
          ShowMonome("Y",mDegY[aK]);
          // if (mDegX[aK]) std::cout << "X^" << mDegX[aK];
          // if (mDegY[aK]) std::cout << "Y^" << mDegY[aK];
          std::cout << "      ADR=" << &aCoef[aK] << "\n";
          std::cout << "\n";
     }
}


void TestBGC3M2D()
{
/*
   std::string aName =  "/media/data2/Jeux-Test/Dino/Ori-Martini/Orientation-_MG_0140.CR2.xml";
   CamStenope * aCS = BasicCamOrientGenFromFile(aName);

   
   cPolynomial_BGC3M2D  aP1(aCS,1,0.0);
   cPolynomial_BGC3M2D  aP2(aCS,0,0.0);
   cPolynomial_BGC3M2D  aP2Bis(aCS,3,0.0);
   cPolynomial_BGC3M2D  aP3(aCS,2,0.0);
   cPolynomial_BGC3M2D  aP4(aCS,1,0.0);
*/
}

void GenCodeEqProjGen(int aDeg,bool GenCode,bool GenCodeAttach,bool GenCodeRot)
{
    cSetEqFormelles  * aSet = new cSetEqFormelles(cNameSpaceEqF::eSysPlein);
    std::vector<double> aPAF;
    CamStenopeIdeale aCSI(false,1.0,Pt2dr(0,0),aPAF);
    aCSI.SetSz(Pt2di(100,100));

    cPolynomial_BGC3M2D aPolCSI(&aCSI,"Test","tutu",aDeg,0.0);

    new cPolynBGC3M2D_Formelle(*aSet,aPolCSI,GenCode,GenCodeAttach,GenCodeRot);
}

/***********************************************************************/
/*                                                                     */
/*                                                                     */
/*                                                                     */
/***********************************************************************/




/*Footer-MicMac-eLiSe-25/06/2007

Ce logiciel est un programme informatique servant à la mise en
correspondances d'images pour la reconstruction du relief.

Ce logiciel est régi par la licence CeCILL-B soumise au droit français et
respectant les principes de diffusion des logiciels libres. Vous pouvez
utiliser, modifier et/ou redistribuer ce programme sous les conditions
de la licence CeCILL-B telle que diffusée par le CEA, le CNRS et l'INRIA 
sur le site "http://www.cecill.info".

En contrepartie de l'accessibilité au code source et des droits de copie,
de modification et de redistribution accordés par cette licence, il n'est
offert aux utilisateurs qu'une garantie limitée.  Pour les mêmes raisons,
seule une responsabilité restreinte pèse sur l'auteur du programme,  le
titulaire des droits patrimoniaux et les concédants successifs.

A cet égard  l'attention de l'utilisateur est attirée sur les risques
associés au chargement,  à l'utilisation,  à la modification et/ou au
développement et à la reproduction du logiciel par l'utilisateur étant 
donné sa spécificité de logiciel libre, qui peut le rendre complexe à 
manipuler et qui le réserve donc à des développeurs et des professionnels
avertis possédant  des  connaissances  informatiques approfondies.  Les
utilisateurs sont donc invités à charger  et  tester  l'adéquation  du
logiciel à leurs besoins dans des conditions permettant d'assurer la
sécurité de leurs systèmes et ou de leurs données et, plus généralement, 
à l'utiliser et l'exploiter dans les mêmes conditions de sécurité. 

Le fait que vous puissiez accéder à cet en-tête signifie que vous avez 
pris connaissance de la licence CeCILL-B, et que vous en avez accepté les
termes.
Footer-MicMac-eLiSe-25/06/2007*/
