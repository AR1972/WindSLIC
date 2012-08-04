#include <efi.h>
#include "libefi.h"
//
#define  ASSERT(a)
//
EFI_STATUS
WaitForSingleEvent (
    IN EFI_EVENT        Event,
    IN UINT64           Timeout OPTIONAL
    )
{
    EFI_STATUS          Status;
    UINTN               Index;
    EFI_EVENT           TimerEvent;
    EFI_EVENT           WaitList[2];

    if (Timeout) {
        //
        // Create a timer event
        //

        Status = BS->CreateEvent (EVT_TIMER, 0, (EFI_EVENT_NOTIFY)NULL, NULL, &TimerEvent);
        if (!EFI_ERROR(Status)) {

            //
            // Set the timer event
            //

            BS->SetTimer (TimerEvent, TimerRelative, Timeout);
            
            //
            // Wait for the original event or the timer
            //

            WaitList[0] = Event;
            WaitList[1] = TimerEvent;
            Status = BS->WaitForEvent (2, WaitList, &Index);
            BS->CloseEvent (TimerEvent);

            //
            // If the timer expired, change the return to timed out
            //

            if (!EFI_ERROR(Status)  &&  Index == 1) {
                Status = EFI_TIMEOUT;
            }
        }

    } else {

        //
        // No timeout... just wait on the event
        //

        Status = BS->WaitForEvent (1, &Event, &Index);
        ASSERT (!EFI_ERROR(Status));
        ASSERT (Index == 0);
    }

    return Status;
}
//