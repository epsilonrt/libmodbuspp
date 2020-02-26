/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libmodbuspp Library.
 *
 * The libmodbuspp Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libmodbuspp Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libmodbuspp Library; if not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <memory>

/*
 * Internal: Pointer to implementation
 * "Pointer to implementation" or "pImpl" is a C++ programming technique that
 * removes implementation details of a class from its object representation by
 * placing them in a separate class, accessed through an opaque pointer.
 * This technique is used to construct C++ library interfaces with stable
 * ABI and to reduce compile-time dependencies.
 */
#define PIMP_D(Class) Class::Private * const d = d_func()
#define PIMP_Q(Class) Class * const q = q_func()
#define PIMP_DECLARE_PRIVATE(Class)\
  inline Class::Private* d_func() {\
    return reinterpret_cast<Class::Private*>(d_ptr.get());\
  }\
  inline const Class::Private* d_func() const {\
    return reinterpret_cast<const Class::Private *>(d_ptr.get());\
  }\
  friend class Class::Private;
#define PIMP_DECLARE_PUBLIC(Class) \
  inline Class* q_func() { return reinterpret_cast<Class *>(q_ptr); } \
  inline const Class* q_func() const { return reinterpret_cast<const Class *>(q_ptr); } \
  friend class Class;

/* ========================================================================== */
