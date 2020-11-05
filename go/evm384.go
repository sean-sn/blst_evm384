// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

package evm384

func AddMod384(ret, a, b, p *[6]uint64) {
	add_mod_384(ret, a, b, p)
}

func SubMod384(ret, a, b, p *[6]uint64) {
	sub_mod_384(ret, a, b, p)
}

func MulMod384(ret, a, b, p *[6]uint64, inv uint64) {
	mul_mod_384(ret, a, b, p, inv)
}
