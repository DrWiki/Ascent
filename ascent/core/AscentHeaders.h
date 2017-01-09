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

#include "ascent/Recorder.h"
#include "ascent/Sim.h"
#include "ascent/State.h"

// Timing
#include "ascent/timing/Clock.h"
#include "ascent/timing/Sampler.h"

// Integrators
#include "ascent/integrators/Euler.h"
#include "ascent/integrators/RK2.h"
#include "ascent/integrators/RK4.h"
#include "ascent/integrators/RKMM.h"

// Linear Algebra
#include "ascent/StateVector.h"

#include <deque>
#include <string>