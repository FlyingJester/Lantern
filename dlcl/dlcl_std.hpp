/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/ */

#pragma once
#include "dlcl_parser.hpp"


namespace DarkLight {
namespace CL{
namespace Std{

bool Max(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Min(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Sum(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Any(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool All(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Len(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Empty(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);
bool Print(char *err, Value &return_val, void *arg, Value *args, unsigned num_args);

} // namespace Std
} // namespace CL
} // namespace DarkLight
