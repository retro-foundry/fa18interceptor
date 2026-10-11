# OS and ROM boundaries during recompilation

[Recompiler index](README.md) · [Hardware and service contracts](../decompilation/07-hardware-os-and-audio.md)

Determine which OS/ROM services the actual game calls before choosing a runtime
boundary. A snapshot after hardware takeover can reduce dependencies, but does
not prove their absence. Later disk/resource operations, callbacks and teardown
may still call library services. Preserve these as explicit inventory entries.

The shared recompiler retains guest registers, addresses and library-call ABI.
An Amiga library call usually resolves through a negative vector from its base;
record the live base, entry, register/stack arguments, caller and returned effects.
Use bounded original service captures rather than deriving meaning from an LVO
name alone. Allocator/task/interrupt ordering can affect future behavior.

## Native service replacement

A project's native shim must implement the services it actually exercises,
including caller-visible registers/flags, memory ownership, waits, callbacks
and ordered hardware effects. Declare hook entry boundaries and test the live
path. Unsupported services must report the missing contract; successful no-ops
cannot qualify a port.

The inherited architecture describes reading NDK `.fd` metadata to generate
vector/register tables. No `arecomp osshim` command or general shim generator
exists in the imported revision. A user-supplied local NDK can be reference
material; it is not included. Method/specification is distinct from implementation.

## Supplied ROM route

The guest memory model can map a user-supplied ROM read-only. Recompiled or
development-interpreted ROM execution is still a ROM/CPU dependency. Record
the actual original ROM/config identity and what paths require it. A snapshot
containing already-initialized OS state does not establish cold launch or ROM
independence. No game media or ROM belongs in the shared runtime.
