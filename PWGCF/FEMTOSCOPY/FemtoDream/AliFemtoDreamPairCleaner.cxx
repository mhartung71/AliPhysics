/*
 * AliFemtoDreamPairCleaner.cxx
 *
 *  Created on: 8 Nov 2017
 *      Author: bernhardhohlweger
 */

#include <iostream>
#include "AliFemtoDreamPairCleaner.h"
ClassImp(AliFemtoDreamPairCleaner)
AliFemtoDreamPairCleaner::AliFemtoDreamPairCleaner()
:fMinimalBooking(false)
,fHists(0)
{
}
AliFemtoDreamPairCleaner::AliFemtoDreamPairCleaner(
    int nTrackDecayChecks, int nDecayDecayChecks,bool MinimalBooking)
{
  fMinimalBooking=MinimalBooking;
  if (!fMinimalBooking) {
    fHists=new AliFemtoDreamPairCleanerHists(
        nTrackDecayChecks,nDecayDecayChecks,2);
  }
}

AliFemtoDreamPairCleaner::~AliFemtoDreamPairCleaner() {
  if (fHists) {
    delete fHists;
  }
}

void AliFemtoDreamPairCleaner::CleanTrackAndDecay(
    std::vector<AliFemtoDreamBasePart> *Tracks,
    std::vector<AliFemtoDreamBasePart> *Decay, int histnumber)
{
  int counter=0;
  for (auto itTrack=Tracks->begin();itTrack!=Tracks->end();++itTrack) {
    //std::cout  << "New Track" << std::endl;
    for (auto itDecay=Decay->begin();itDecay!=Decay->end();++itDecay) {
      if (itDecay->UseParticle()) {
        //std::cout  << "New v0" << std::endl;
        std::vector<int> IDTrack=itTrack->GetIDTracks();
        std::vector<int> IDDaug=itDecay->GetIDTracks();
        for (auto itIDs=IDDaug.begin();itIDs!=IDDaug.end();++itIDs) {
          //std::cout <<"ID of Track: "<<IDTrack.at(0)<<" IDs of Daughter: "
//              <<*itIDs<<'\n';
          if (*itIDs==IDTrack.at(0)) {
            itDecay->SetUse(false);
            counter++;
          }
        }
      } else {
        continue;
      }
    }
  }
  if (!fMinimalBooking) fHists->FillDaughtersSharedTrack(histnumber,counter);
}
void AliFemtoDreamPairCleaner::CleanDecayAndDecay(
    std::vector<AliFemtoDreamBasePart> *Decay1,
    std::vector<AliFemtoDreamBasePart> *Decay2, int histnumber) {
  int counter=0;
  for (auto itDecay1=Decay1->begin();itDecay1!=Decay1->end();++itDecay1) {
    if (itDecay1->UseParticle()) {
      for (auto itDecay2=Decay2->begin();itDecay2!=Decay2->end();++itDecay2) {
        if (itDecay1->UseParticle()) {
          std::vector<int> IDDaug1=itDecay1->GetIDTracks();
          std::vector<int> IDDaug2=itDecay2->GetIDTracks();
          for (auto itID1s=IDDaug1.begin();itID1s!=IDDaug1.end();++itID1s) {
            for (auto itID2s=IDDaug2.begin();itID2s!=IDDaug2.end();++itID2s) {
              if (*itID1s==*itID2s) {
                if (itDecay1->GetCPA() < itDecay2->GetCPA()) {
                  itDecay1->SetUse(false);
                  counter++;
                } else {
                  itDecay2->SetUse(false);
                  counter++;
                }
              }
            }
          }
        } else {
          continue;
        }
      }
    } else {
      continue;
    }
  }
  if (!fMinimalBooking) fHists->FillDaughtersSharedDaughter(histnumber,counter);
}

void AliFemtoDreamPairCleaner::CleanDecay(
    std::vector<AliFemtoDreamBasePart> *Decay,int histnumber)
{
  int counter=0;
  for (std::vector<AliFemtoDreamBasePart>::iterator itDecay1=Decay->begin();
      itDecay1!=Decay->end();++itDecay1) {
    if (itDecay1->UseParticle()) {
      //std::cout  << "New Particle 1" << std::endl;
      for (auto itDecay2=itDecay1+1;itDecay2!=Decay->end();++itDecay2) {
        if (itDecay2->UseParticle()) {
          //std::cout  << "New Particle 2" << std::endl;
          std::vector<int> IDDaug1=itDecay1->GetIDTracks();
          std::vector<int> IDDaug2=itDecay2->GetIDTracks();
          for (auto itID1s=IDDaug1.begin();itID1s!=IDDaug1.end();++itID1s) {
            for (auto itID2s=IDDaug2.begin();itID2s!=IDDaug2.end();++itID2s) {
              //std::cout <<"ID of Daug v01: "<<*itID1s<<" ID of Daug v02: "
//                  <<*itID2s<<'\n';
              if (*itID1s==*itID2s) {
                if (itDecay1->GetCPA() < itDecay2->GetCPA()) {
                  itDecay1->SetUse(false);
                  counter++;
                } else {
                  itDecay2->SetUse(false);
                  counter++;                }
              }
            }
          }
        } else {
          continue;
        }
      }
    } else {
      continue;
    }
  }
  if (!fMinimalBooking) fHists->FillDaughtersSharedDaughter(histnumber,counter);
}

void AliFemtoDreamPairCleaner::StoreParticle(
    std::vector<AliFemtoDreamBasePart> Particles)
{
  int counter=0;
  std::vector<AliFemtoDreamBasePart> tmpParticles;
  for (auto itPart:Particles) {
    if (itPart.UseParticle()) {
      tmpParticles.push_back(itPart);
    } else {
      counter++;
    }
  }
  fParticles.push_back(tmpParticles);
}
void AliFemtoDreamPairCleaner::ResetArray() {
  fParticles.clear();
}

void AliFemtoDreamPairCleaner::FillInvMassPair(
    std::vector<AliFemtoDreamBasePart> &Part1, int PDGCode1,
    std::vector<AliFemtoDreamBasePart> &Part2, int PDGCode2,
    int histnumber)
{
  for (const auto &it1:Part1) {
    for (const auto &it2:Part2) {
      float invMass=InvMassPair(it1.GetMomentum(),PDGCode1,
                                 it2.GetMomentum(),PDGCode2);
//      std::cout << invMass << std::endl;
      fHists->FillPairInvMass(histnumber,invMass);
    }
  }
}


