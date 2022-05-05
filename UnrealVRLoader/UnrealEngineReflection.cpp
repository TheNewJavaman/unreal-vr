#include "UnrealEngineReflection.h"

#include "PatternStreams.h"

namespace UnrealVr::UE {
    void UObject::ProcessEvent_t::operator()(UFunction* Function, void* Params) {
        if (Target == nullptr) {
            const auto match = PS::PatternStream(
                    {
                        0x40, 0x55, // push rbp
                        0x56,       // push rsi
                        0x57,       // push rdi
                        0x41, 0x54, // push r12
                        0x41, 0x55, // push r13
                        0x41, 0x56, // push r14
                        0x41, 0x57  // push r15
                    }
                )
                .HasPatternInRange(
                    {
                        0x4D, 0x8B, 0xF8,                  // mov r15, r8
                        0x48, 0X8B, 0xF2,                  // mov rsi, rdx
                        0x4C, 0x8B, 0xE1,                  // mov r12, rcx
                        0x41, 0xB8, 0xFF, 0xFF, 0x00, 0x00 // mov r8d, 0x0000FFFF
                    }, 0, 0x40, false
                )
                .HasPatternInRange(
                    {
                        0x48, 0x98,            // cdqe
                        0xC1, 0xF9, 0x10,      // sar ecx, 0x10
                        0x48, 0x63, 0xC9,      // movsxd rcx, ecx
                        0x48, 0x8B, 0x0C, 0xC8 // lea rdx, [rax + rax * 2]
                    }, 0, 0x80, false
                )
                .FirstOrNullPtr();
            if (match == nullptr) {
                logger->Error("Couldn't find target");
                return;
            }
            logger->Info("Found target at {:p}", match);
            Target = reinterpret_cast<Target_t*>(match);
        }
        Target(parent, Function, Params);
    }
}
