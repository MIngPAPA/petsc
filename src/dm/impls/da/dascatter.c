/*
  Code for manipulating distributed regular arrays in parallel.
*/

#include <petsc/private/dmdaimpl.h> /*I   "petscdmda.h"   I*/

/*@
  DMDAGetScatter - Gets the global-to-local, and
  local-to-local vector scatter contexts for a `DMDA` distributed array.

  Collective

  Input Parameter:
. da - the `DMDA`

  Output Parameters:
+ gtol - global-to-local scatter context (may be `NULL`)
- ltol - local-to-local scatter context (may be `NULL`)

  Level: developer

  Note:
  The output contexts are valid only as long as the input `da` is valid.
  If you delete the `da`, the scatter contexts will become invalid.

.seealso: [](sec_struct), `DM`, `DMDA`, `DMGlobalToLocalBegin()`, `DMGlobalToLocalEnd()`, `DMLocalToGlobalBegin()`
@*/
PetscErrorCode DMDAGetScatter(DM da, PeOp VecScatter *gtol, PeOp VecScatter *ltol)
{
  DM_DA *dd = (DM_DA *)da->data;

  PetscFunctionBegin;
  PetscValidHeaderSpecificType(da, DM_CLASSID, 1, DMDA);
  if (gtol) *gtol = dd->gtol;
  if (ltol) {
    if (!dd->ltol) PetscCall(DMLocalToLocalCreate_DA(da));
    *ltol = dd->ltol;
  }
  PetscFunctionReturn(PETSC_SUCCESS);
}
