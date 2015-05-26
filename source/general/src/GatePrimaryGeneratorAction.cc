/*----------------------
   Copyright (C): OpenGATE Collaboration

This software is distributed under the terms
of the GNU Lesser General  Public Licence (LGPL)
See GATE/LICENSE.txt for further details
----------------------*/


#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleGun.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
#include "G4ios.hh"

#include "GatePrimaryGeneratorAction.hh"
#include "GatePrimaryGeneratorMessenger.hh"
#include "GateDetectorConstruction.hh"

#include "GateClock.hh"
#include "GateApplicationMgr.hh"

#include "GateSourceMgr.hh"
//#include "GateOutputMgr.hh"
//#include "GateHitFileReader.hh"

#include "GateConfiguration.h"

//---------------------------------------------------------------------------
GatePrimaryGeneratorAction::GatePrimaryGeneratorAction()
{
  m_messenger = new GatePrimaryGeneratorMessenger(this);
  m_printModulo  = 100;
  m_nVerboseLevel = 0;
  m_particleGun  = 0;//new G4GeneralParticleSource();
  m_useGPS = false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
GatePrimaryGeneratorAction::~GatePrimaryGeneratorAction()
{
  delete m_messenger;
  if(m_particleGun) delete m_particleGun;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void GatePrimaryGeneratorAction::SetVerboseLevel(G4int value) 
{ 
  m_nVerboseLevel = value; 
  if (m_nVerboseLevel == 0) {
    m_printModulo  = 1000;
  } else if (m_nVerboseLevel == 1) {
    m_printModulo  = 100;
  } else if (m_nVerboseLevel == 2) {
    m_printModulo  = 10;
  } else if (m_nVerboseLevel == 3) {
    m_printModulo  = 1;
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void GatePrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //  if (GateOutputMgr::GetInstance()->GetDigiMode() == kruntimeMode)
  //  else
  //    GenerateDigitisationPrimaries(event);
  if (!m_useGPS) {
    GenerateSimulationPrimaries(event);
  }
  else {
    static int i=0;
    GateMessage("Beam", 2, "Generating particle " << i << Gateendl);
    // m_particleGun->GeneratePrimaryVertex(event);
    //    DD(m_particleGun->GetNumberOfParticles());
    //    DD(m_particleGun->GetNumberofSource());
    //    DD(m_particleGun->GetCurrentSource()->GetNumberOfParticles());
    //    DD(m_particleGun->GetCurrentSource()->GetParticleTime());
    ++i;
    /* GateMessage("Beam", 2, "(" << i << ") " << m_particleGun->GetParticleDefinition()->GetParticleName() 
       << " pos.=" << m_particleGun->GetParticlePosition()
       << " energy=" <<  m_particleGun->GetParticleEnergy() 
       << " dir.=" << m_particleGun->GetParticleMomentumDirection()
       << ")\n"); */ 
  }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void GatePrimaryGeneratorAction::GenerateSimulationPrimaries(G4Event* event)
{
  //! compute the right number of events per slice at this time 
  G4int eventID = event->GetEventID();
  GateSourceMgr* sourceMgr = GateSourceMgr::GetInstance();
  if (eventID==0) {
    const G4Run* currentRun = GateRunManager::GetRunManager()->GetCurrentRun();
    //if( currentRun->GetRunID()==0) sourceMgr->Initialization();
    sourceMgr->PrepareNextRun( currentRun );
    m_nEvents=0;
  }

  G4int numVertices = sourceMgr->PrepareNextEvent(event);  
  //! stop the run if no particle has been generated by the source manager
  if (numVertices == 0) {
    GateRunManager* runManager = GateRunManager::GetRunManager();

    runManager->AbortRun(true);
    if (m_nVerboseLevel>1) G4cout << "GatePrimaryGeneratorAction::GeneratePrimaries: numVertices == 0, run aborted \n";  
  } 
  else {
    m_nEvents++;
    G4PrimaryParticle  * p = event->GetPrimaryVertex(0)->GetPrimary(0);
//    if(sourceMgr->GetWeight()>0)  event->GetPrimaryVertex()->SetWeight(sourceMgr->GetWeight());
     if(sourceMgr->GetWeight()>0)  p->SetWeight(sourceMgr->GetWeight());
   
    if (m_nVerboseLevel>0) {
      if ((m_nEvents%m_printModulo) == 0) {
	G4cout << "GatePrimaryGeneratorAction::GeneratePrimaries: m_nEvents " << m_nEvents << Gateendl;  
      }
    }
  }
}
//---------------------------------------------------------------------------

/*
//---------------------------------------------------------------------------
void GatePrimaryGeneratorAction::GenerateDigitisationPrimaries(G4Event* event)
{
  
#ifdef G4ANALYSIS_USE_ROOT
GateHitFileReader* digiSource = GateHitFileReader::GetInstance();

G4int numEvent = digiSource->PrepareNextEvent(event);


//! stop the run if no particle has been generated by the source manager
if (numEvent == 0) {
GateRunManager* runManager = GateRunManager::GetRunManager();

runManager->AbortRun(true);
if (m_nVerboseLevel>1) G4cout << "GatePrimaryGeneratorAction::GeneratePrimaries: numVertices == 0, run aborted \n";  
} else {
m_nEvents++;
if (m_nVerboseLevel>0) {
if ((m_nEvents%m_printModulo) == 0) {
G4cout << "GatePrimaryGeneratorAction::GeneratePrimaries: m_nEvents " << m_nEvents << Gateendl;  
}
}
}
#endif
}
*/
 //---------------------------------------------------------------------------

 //---------------------------------------------------------------------------
void GatePrimaryGeneratorAction::AddEvent()
{
  if (m_nVerboseLevel>0) { 
    if ((m_nEvents%m_printModulo) == 0) {
      G4cout << "GatePrimaryGeneratorAction::AddEvent: events " << m_nEvents << Gateendl;
    }
  }
}
//---------------------------------------------------------------------------



