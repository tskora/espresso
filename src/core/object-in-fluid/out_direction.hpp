/*
  Copyright (C) 2012-2018 The ESPResSo project

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _OBJECT_IN_FLUID_OUT_DIRECTION_H
#define _OBJECT_IN_FLUID_OUT_DIRECTION_H
/** \file
 * Routines to calculate the outward direction of the
 * membrane using a particle quadruple (one particle and its 3 strategically
 * placed neighbors)
 */

#include "config.hpp"

#ifdef MEMBRANE_COLLISION

#include "bonded_interactions/bonded_interaction_data.hpp"
#include "grid.hpp"
#include "particle_data.hpp"
#include "utils.hpp"

// set out_direction parameters
int oif_out_direction_set_params(int bond_type);

/** Computes the outward direction of the membrane from one particle and its
   three neighbors

    @param p1           Pointer to the central particle.
    @param p2,p3,p4     Pointers to the neighboring particles.

    computes the normal of triangle p2p3p4
    this triangle was initially oriented in such a way that its normal already
   points out of the object normalizes and stores the result as out_direction in
   p1 data
    @return 0
*/
inline int calc_out_direction(
    Particle *p1, Particle *p2, Particle *p3, Particle *p4,
    Bonded_ia_parameters *iaparams) // first-fold-then-the-same approach
{
  double n[3], dn;
  int j;
  Vector3d fp1, fp2, fp3, fp4;
  double AA[3], BB[3], CC[3];

  // first find out which particle out of p1, p2 (possibly p3, p4) is not a
  // ghost particle. In almost all cases it is p2, however, it might be other
  // one. we call this particle reference particle.
  if (p2->l.ghost != 1) {
    // unfold non-ghost particle using image, because for physical particles,
    // the structure p->l.i is correctly set
    fp2 = unfolded_position(p2);
    // other coordinates are obtained from its relative positions to the
    // reference particle
    get_mi_vector(AA, p1->r.p, fp2);
    get_mi_vector(BB, p3->r.p, fp2);
    get_mi_vector(CC, p4->r.p, fp2);
    for (int i = 0; i < 3; i++) {
      fp1[i] = fp2[i] + AA[i];
      fp3[i] = fp2[i] + BB[i];
      fp4[i] = fp2[i] + CC[i];
    }
  } else {
    // in case  particle p2 is a ghost particle
    if (p1->l.ghost != 1) {
      fp1 = unfolded_position(p1);
      get_mi_vector(AA, p2->r.p, fp1);
      get_mi_vector(BB, p3->r.p, fp1);
      get_mi_vector(CC, p4->r.p, fp1);
      for (int i = 0; i < 3; i++) {
        fp2[i] = fp1[i] + AA[i];
        fp3[i] = fp1[i] + BB[i];
        fp4[i] = fp1[i] + CC[i];
      }
    } else {
      // in case the first and the second particle are ghost particles
      if (p3->l.ghost != 1) {
        fp3 = unfolded_position(p3);
        get_mi_vector(AA, p1->r.p, fp3);
        get_mi_vector(BB, p2->r.p, fp3);
        get_mi_vector(CC, p4->r.p, fp3);
        for (int i = 0; i < 3; i++) {
          fp1[i] = fp3[i] + AA[i];
          fp2[i] = fp3[i] + BB[i];
          fp4[i] = fp3[i] + CC[i];
        }
      } else {
        // in case the first and the second particle are ghost particles
        if (p4->l.ghost != 1) {
          fp4 = unfolded_position(p4);
          get_mi_vector(AA, p1->r.p, fp4);
          get_mi_vector(BB, p2->r.p, fp4);
          get_mi_vector(CC, p3->r.p, fp4);
          for (int i = 0; i < 3; i++) {
            fp1[i] = fp4[i] + AA[i];
            fp2[i] = fp4[i] + BB[i];
            fp3[i] = fp4[i] + CC[i];
          }
        } else {
          printf("Something wrong in out_direction.hpp: All particles in a "
                 "bond are ghost particles, impossible to "
                 "unfold the positions...");
          return 0;
        }
      }
    }
  }

  get_n_triangle(fp2, fp3, fp4, n);
  dn = normr(n);
  if (fabs(dn) < 0.001)
    printf("out_direction.hpp, calc_out_direction: Length of outward vector is "
           "close to zero!\n");
  for (j = 0; j < 3; j++) {
    p1->p.out_direction[j] = n[j] / dn;
  }
  return 0;
}

#endif /* ifdef MEMBRANE_COLLISION */
#endif
