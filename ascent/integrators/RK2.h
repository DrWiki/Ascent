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

// Second order, two pass Runge Kutta.

namespace asc
{
   template <typename C>
   struct RK2T
   {
      using value_t = typename C::value_type;

      template <typename System>
      void operator()(System&& system, C& x, value_t& t, const value_t dt)
      {
         t0 = t;
         dt_2 = half*dt;

         n = x.size();
         if (xd.size() < n)
            xd.resize(n);

         for (size_t pass = 0; pass < 2; ++pass)
         {
            switch (pass)
            {
            case 0:
               x0 = x;
               system(x, xd, t);
               core::propagate(x, dt_2, xd, x0);
               t += dt_2;
               break;
            case 1:
               system(x, xd, t);
               core::propagate(x, dt, xd, x0);
               t = t0 + dt;
               break;
            default:
               break;
            }
         }
      }

   private:
      static constexpr auto half = static_cast<value_t>(0.5);
      value_t t0, dt_2;
      size_t n;
      C x0, xd;
   };
}