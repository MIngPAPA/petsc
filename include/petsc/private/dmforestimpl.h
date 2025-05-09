#pragma once

#include <petscmat.h>      /*I      "petscmat.h"          I*/
#include <petscdmforest.h> /*I      "petscdmforest.h"    I*/
#include <petscbt.h>
#include <petsc/private/dmimpl.h>

typedef struct {
  PetscInt refct;
  void    *data;
  PetscErrorCode (*clearadaptivityforest)(DM);
  PetscErrorCode (*getadaptivitysuccess)(DM, PetscBool *);
  PetscErrorCode (*transfervec)(DM, Vec, DM, Vec, PetscBool, PetscReal);
  PetscErrorCode (*transfervecfrombase)(DM, Vec, Vec);
  PetscErrorCode (*createcellchart)(DM, PetscInt *, PetscInt *);
  PetscErrorCode (*createcellsf)(DM, PetscSF *);
  PetscErrorCode (*destroy)(DM);
  PetscErrorCode (*ftemplate)(DM, DM);
  PetscBool computeAdaptSF;
  PetscErrorCode (*mapcoordinates)(DM, PetscInt, PetscInt, const PetscReal[], PetscReal[], void *);
  void                      *mapcoordinatesctx;
  DMForestTopology           topology;
  DM                         base;
  DM                         adapt;
  DMAdaptFlag                adaptPurpose;
  PetscInt                   adjDim;
  PetscInt                   overlap;
  PetscInt                   minRefinement;
  PetscInt                   maxRefinement;
  PetscInt                   initRefinement;
  PetscInt                   cStart;
  PetscInt                   cEnd;
  PetscSF                    cellSF;
  PetscSF                    preCoarseToFine;
  PetscSF                    coarseToPreFine;
  DMLabel                    adaptLabel;
  DMForestAdaptivityStrategy adaptStrategy;
  PetscInt                   gradeFactor;
  PetscReal                 *cellWeights;
  PetscCopyMode              cellWeightsCopyMode;
  PetscReal                  weightsFactor;
  PetscReal                  weightCapacity;
} DM_Forest;

PETSC_EXTERN PetscErrorCode DMCreate_Forest(DM);
PETSC_INTERN PetscErrorCode DMClone_Forest(DM, DM *);
PETSC_INTERN PetscErrorCode DMSetFromOptions_Forest(DM, PetscOptionItems);
PETSC_INTERN PetscErrorCode DMAdaptLabel_Forest(DM, Vec, DMLabel, DMLabel, DM *);
