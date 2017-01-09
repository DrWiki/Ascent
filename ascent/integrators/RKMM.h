// Copyright (c) 2016-2017 Anyar, Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//      http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ascent/core/Propagate.h"

// Five pass Runge Kutta Merson's Method. With adaptive time stepping.

namespace asc
{
   namespace core
   {
      template <typename C>
      typename C::value_type maxAbsDiff(const C& x0, const C& x1)
      {
         typename C::value_type maximum{};
         typename C::value_type temp;
         const size_t n = x0.size();
         for (size_t i = 0; i < n; ++i)
         {
            temp = abs(x0[i] - x1[i]);
            if (temp > maximum)
               maximum = temp;
         }

         return maximum;
      }
   }

   template <typename C>
   struct RKMMT
   {
      using value_t = typename C::value_type;

      // epsilon is the error tolerance
      RKMMT(const value_t epsilon) : epsilon(epsilon), epsilon_64(epsilon / static_cast<value_t>(64.0)) {}

      template <typename System>
      void operator()(System&& system, C& x, value_t& t, value_t& dt)
      {
         t0 = t;
         dt_2 = half*dt;
         dt_3 = third*dt;
         dt_6 = sixth*dt;
         dt_8 = eigth*dt;

         const size_t n = x.size();
         if (xd0.size() < n)
         {
            xd0.resize(n);
            xd1.resize(n);
            xd2.resize(n);
            xd3.resize(n);
            xd4.resize(n);
            x_estimate.resize(n);
         }

         for (size_t pass = 0; pass < 5; ++pass)
         {
            switch (pass)
            {
            case 0:
               x0 = x;
               system(x, xd0, t);
               core::propagate(x, dt_3, xd0, x0);
               t += dt_3;
               break;
            case 1:
               system(x, xd1, t);
               core::propagate(x, dt_6, xd0, xd1, x0);
               break;
            case 2:
               system(x, xd2, t);
#ifdef ASCENT_NO_FMA
               for (size_t i = 0; i < n; ++i)
                  x[i] = dt_8 * (xd0[i] + 3.0*xd2[i]) + x0[i];
#else
               for (size_t i = 0; i < n; ++i)
                  x[i] = fma(dt_8, xd0[i] + 3.0*xd2[i], x0[i]);
#endif
               t = t0 + dt_2;
               break;
            case 3:
               system(x, xd3, t);
#ifdef ASCENT_NO_FMA
               for (size_t i = 0; i < n; ++i)
                  x[i] = dt_2 * (xd0[i] - 3.0*xd2[i] + 4.0*xd3[i]) + x0[i];
#else
               for (size_t i = 0; i < n; ++i)
                  x[i] = fma(dt_2, xd0[i] - 3.0*xd2[i] + 4.0*xd3[i], x0[i]);
#endif
               t = t0 + dt;
               break;
            case 4:
               system(x, xd4, t);
#ifdef ASCENT_NO_FMA
               for (size_t i = 0; i < n; ++i)
                  x[i] = dt_6 * (xd0[i] + 4.0*xd3[i] + xd4[i]) + x0[i];
#else
               for (size_t i = 0; i < n; ++i)
                  x[i] = fma(dt_6, xd0[i] + 4.0*xd3[i] + xd4[i], x0[i]);
#endif

               // Adaptive time stepping would probably be best handled by a constexpr if, because we want the handling to be determined at compile time, perhaps by a templated boolean
               // https://www.encyclopediaofmath.org/index.php/Kutta-Merson_method#Eq-2

#ifdef ASCENT_NO_FMA
               for (size_t i = 0; i < n; ++i)
                  x_estimate[i] = dt_2 * (xd0[i] - 3.0*xd2[i] + 4.0*xd3[i]) + x0[i];
#else
               for (size_t i = 0; i < n; ++i)
                  x_estimate[i] = fma(dt_2, xd0[i] - 3.0*xd2[i] + 4.0*xd3[i], x0[i]);
#endif
               
               R = fifth * core::maxAbsDiff(x_estimate, x); // error estimate

               if (R > epsilon)
               {
                  dt *= half; // reduce the time step
                  t = t0;
                  x = x0;
                  operator()(system, x, t, dt); // recompute the solution recursively
               }
               else if (R <= epsilon_64)
                  dt *= two; // increase the time step for future steps

               break;
            default:
               break;
            }
         }
      }

   private:
      static constexpr auto two = static_cast<value_t>(2.0);
      static constexpr auto fifth = static_cast<value_t>(1.0 / 5.0);

      static constexpr auto half = static_cast<value_t>(0.5);
      static constexpr auto third = static_cast<value_t>(1.0 / 3.0);
      static constexpr auto sixth = static_cast<value_t>(1.0 / 6.0);
      static constexpr auto eigth = static_cast<value_t>(1.0 / 8.0);

      value_t t0, dt_2, dt_3, dt_6, dt_8;
      C x0, xd0, xd1, xd2, xd3, xd4;
      value_t R;
      const value_t epsilon, epsilon_64;
      C x_estimate;
   };
}