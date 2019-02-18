#ifndef SDsharedInfo_hh_included
#define SDsharedInfo_hh_included 1 

#include "sys/time.h" // Or should I use the XDAQ toobox version?
#include <stdint.h>
#include <string.h>

namespace hcal{
  namespace calib{
    namespace sd{

      enum DriverStatusEnum{
        SD_STATUS_ERROR =   -1,
        SD_STATUS_INACTIVE = 0,
        SD_STATUS_IDLE =     1,
        SD_STATUS_BUSY =     2
      };

      static const unsigned int MAX_DRIVERS=32;

      // Bit masks to be applied to DriverAllInfoStruct.status.
      static int32_t fatal_error_mask=0x80000000;
      static int32_t busy_mask=0x08000000;
      static int32_t active_mask=0x10000000;
      static int32_t last_command_status_mask=0x07000000;
      static int32_t in_detector_mask=0x00100000;

      enum last_command_reply{ // Must apply last_command_status_mask first.
        COMMAND_ACCEPTED = 0x00000000,
        COMMAND_IGNORED = 0x01000000,
        COMMAND_FAILED = 0x02000000
      };


      //------------------------------------------------------------------------
      // Data structure for exporting information to "clinets". eg DIM...
      //------------------------------------------------------------------------

      #define DIM_DESCRIPTION "I:10;C:64;C:64;C:64;C:128"
      #define DIM_COMMAND_DESCRIPTION "C:64"

      struct DriverAllInfoStruct{
        int32_t message_counter;       // Increments after each DIM service update.
        int32_t time_stamp_1_sec;
        int32_t time_stamp_1_usec;
        //timeval time_stamp_1;      // (int seconds, int u seconds).
        int32_t status;                // Holds the DIM server status bits.
        int32_t index_counter;         // Driver indexer position (tube number).
        int32_t reel_counter;          // Reel count (mm).
        int32_t motor_current;         // Motor current 5mA/least count.
        int32_t motor_voltage;         // Motor voltage (units?).
        //timeval time_stamp_2;      // Empty place holder(for event builder?).
        int32_t time_stamp_2_sec;
        int32_t time_stamp_2_usec;
	int32_t driver_ID;           // SSID number of driver
	int32_t source_ID;           // SSID number of source in driver
	char connection_point[64];     // Where the driver is physically connected to the tubes.
	char index_description[64];  // Description of current index.
        char last_command[64];     // Echo of last command reveived from outside world (DIM...).
        char message[128];          // Text messages reported from SDmanager. 

        // default contructor
        DriverAllInfoStruct()
        {
          message_counter = -9999;
          time_stamp_1_sec = -9999;
          time_stamp_1_usec = -9999;
          //time_stamp_1 = -9999;
          status = -9999;
          index_counter = -9999;
          reel_counter = -9999;
          motor_current = -9999;
          motor_voltage = -9999;
          //time_stamp_2 = -9999;
          time_stamp_2_sec = -9999;
          time_stamp_2_usec = -9999;
	  driver_ID = 0xFFFFFFFF;
	  source_ID = 0xFFFFFFFF;
          strcpy(connection_point,"UNDEFINED");
          strcpy(index_description,"UNDEFINED");
          strcpy(last_command,"");
          strcpy(message,"");
        }

      };

      //-----------------
      // methods to apply the bitmasks
      inline bool isLastCommandStatusAccepted(DriverAllInfoStruct const& info) {
        return (info.status & last_command_status_mask) == COMMAND_ACCEPTED;
      }
      inline bool isLastCommandStatusFailed(DriverAllInfoStruct const& info) {
        return (info.status & last_command_status_mask) == COMMAND_FAILED;
      }
      inline bool isLastCommandStatusIgnored(DriverAllInfoStruct const& info) {
        return (info.status & last_command_status_mask) == COMMAND_IGNORED;
      }
      inline bool isStatusBusy(DriverAllInfoStruct const& info) {
        return info.status & busy_mask;
      }
      inline bool isStatusFatalError(DriverAllInfoStruct const& info) {
        return info.status & fatal_error_mask;
      }
      inline bool isInDetector(DriverAllInfoStruct const& info) {
        return info.status & in_detector_mask;
      }


    } // End namespace sd
  } // End namespace calib
} // End namespace hcal

#endif
