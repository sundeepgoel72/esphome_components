// Basing this on external_component uart and esphome component DWIN
//-------------------------------------------------------------------
#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/output/float_output.h"

typedef bool boolean;
typedef uint8_t byte;

using std::string;

namespace esphome
{
    namespace dwin
    {

        class Dwin : public Component, public uart::UARTDevice
        {
        public:
            void setup() override;
            void loop() override;
            void dump_config() override;
            void beepHMI();
            void write_vp_command_();
            double getHWVersion();
            void restartHMI();
            void write_vp_command_(uint8_t vp, uint8_t value);
            void setPage(byte pageID);
            byte getPage();
            void setBrightness(byte pConstrast);
            byte getBrightness();
            void readDWIN();
            void setVP(unsigned short int address, byte data);
            //void setText(long address, String textData);

        protected:
            void write_command_(const uint8_t *command);
            void parse_data_();
            uint16_t get_16_bit_uint_(uint8_t start_index) const;
            void handle_char_(uint8_t c);
            std::vector<uint8_t> rx_message_;

        private:
            uint8_t data_[10];
            uint8_t data_index_{0};
            uint32_t last_transmission_{0};
        };

        // class DwinFOutput  : public Component, public output::FloatOutput {
        // public:
        //     DwinFOutput(uint8_t vp);
        //     void dump_config() override;
        //     void set_parent(Dwin *parent) { this->parent_ = parent; }
        // protected:
        //     void write_state(float state) override;
        //     Dwin *parent_;
        //     uint8_t vp;
        // };

    } // namespace dwin
} // namespace esphome
