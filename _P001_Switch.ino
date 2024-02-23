//#######################################################################################################
//#################################### Plugin 001: Input Switch #########################################
//#######################################################################################################

#define PLUGIN_001
#define PLUGIN_ID_001        1

boolean Plugin_001(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;
  static byte switchstate[TASKS_MAX];

  switch (function)
  {

    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_001;
        strcpy(Device[deviceCount].Name, "Switch input");
        Device[deviceCount].Type = DEVICE_TYPE_SINGLE;
        Device[deviceCount].VType = SENSOR_TYPE_SWITCH;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = true;
        Device[deviceCount].InverseLogicOption = true;
        Device[deviceCount].FormulaOption = false;
        Device[deviceCount].ValueCount = 1;
        strcpy(Device[deviceCount].ValueNames[0], "Switch");
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        byte choice = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        String options[2];
        options[0] = F("Switch");
        options[1] = F("Dimmer");
        int optionValues[2];
        optionValues[0] = 1;
        optionValues[1] = 2;
        string += F("<TR><TD>Switch Type:<TD><select name='plugin_001_type'>");
        for (byte x = 0; x < 2; x++)
        {
          string += F("<option value='");
          string += optionValues[x];
          string += "'";
          if (choice == optionValues[x])
            string += " selected";
          string += ">";
          string += options[x];
          string += "</option>";
        }
        string += F("</select>");

        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          char tmpString[80];
          sprintf(tmpString, "<TR><TD>Dim value:<TD><input type='text' name='plugin_001_dimvalue' value='%u'>", Settings.TaskDevicePluginConfig[event->TaskIndex][1]);
          string += tmpString;
        }

        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        String plugin1 = WebServer.arg("plugin_001_type");
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = plugin1.toInt();
        if (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2)
        {
          String plugin2 = WebServer.arg("plugin_001_dimvalue");
          Settings.TaskDevicePluginConfig[event->TaskIndex][1] = plugin2.toInt();
        }

        success = true;
        break;
      }

    case PLUGIN_INIT:
      {
        if (Settings.TaskDevicePin1PullUp[event->TaskIndex])
        {
          Serial.print(F("INIT : InputPullup "));
          Serial.println(Settings.TaskDevicePin1[event->TaskIndex]);
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT_PULLUP);
        }
        else
        {
          Serial.print(F("INIT : Input "));
          Serial.println(Settings.TaskDevicePin1[event->TaskIndex]);
          pinMode(Settings.TaskDevicePin1[event->TaskIndex], INPUT);
        }
        switchstate[event->TaskIndex] = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        success = true;
        break;
      }

    case PLUGIN_TEN_PER_SECOND:
      {
        byte state = digitalRead(Settings.TaskDevicePin1[event->TaskIndex]);
        if (state != switchstate[event->TaskIndex])
        {
          Serial.print(F("SW   : State "));
          Serial.println(state);
          switchstate[event->TaskIndex] = state;
          if (Settings.TaskDevicePin1Inversed[event->TaskIndex])
            state = !state;
          UserVar[event->BaseVarIndex] = state;
          byte SensorType = SENSOR_TYPE_SWITCH;
          if ((state == 1) && (Settings.TaskDevicePluginConfig[event->TaskIndex][0] == 2))
            {
              SensorType = SENSOR_TYPE_DIMMER;
              UserVar[event->BaseVarIndex] = Settings.TaskDevicePluginConfig[event->TaskIndex][1];
            }
          sendData(event->TaskIndex, SensorType, Settings.TaskDeviceID[event->TaskIndex], event->BaseVarIndex);
        }
        success = true;
        break;
      }

  }
  return success;
}
