#include "dwin.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace dwin
    {

        static const char *TAG = "Dwin";

        static const uint8_t CMD_HEAD1 = 0x5A;
        static const uint8_t CMD_HEAD2 = 0xA5;
        static const uint8_t CMD_WRITE = 0x82;
        static const uint8_t CMD_READ = 0x83;

        static const uint8_t MIN_ASCII = 3;
        static const uint8_t MAX_ASCII = 255;

        static const uint8_t CMD_READ_TIMEOUT = 50;
        static const uint8_t READ_TIMEOUT = 100;

        void Dwin::setup()
        {
        }

        void Dwin::loop()
        {
            // this->readDWIN();
            while (this->available())
            {
                uint8_t c;
                this->read_byte(&c);
                this->handle_char_(c);
            }
        }

        void Dwin::handle_char_(uint8_t c)
        {
            if (c == '\r')
                return;
            if (c == '\n')
            {
                std::string s(this->rx_message_.begin(), this->rx_message_.end());

                //if (this->the_text_ != nullptr)
                //    this->the_text_->publish_state(s);
                //if (this->the_sensor_ != nullptr)
                //    this->the_sensor_->publish_state(parse_number<float>(s).value_or(0));
                //if (this->the_binsensor_ != nullptr)
                //    this->the_binsensor_->publish_state(s == "ON");
                
                this->rx_message_.clear();
                return;
            }
            this->rx_message_.push_back(c);
        }

        void Dwin::readDWIN()
        {
            const uint32_t now = millis();
            if ((now - this->last_transmission_ >= 500) && this->data_index_)
            {
                // last transmission too long ago. Reset RX index.
                ESP_LOGV(TAG, "Last transmission too long ago. Reset RX index.");
                this->data_index_ = 0;
            }

            if (this->available() == 0)
            {
                return;
            }

            this->last_transmission_ = now;
            while (this->available() != 0)
            {
                this->read_byte(&this->data_[this->data_index_]);
                // auto check = this->check_byte_();
                // if (!check.has_value())
                if (true)
                {
                    // finished
                    this->parse_data_();
                    this->data_index_ = 0;
                }
                // else if (!*check)
                //{
                //    // wrong data
                //     ESP_LOGV(TAG, "Byte %i of received data frame is invalid.", this->data_index_);
                //     this->data_index_ = 0;
                // }
                else
                {
                    // next byte
                    this->data_index_++;
                }
            }
        }

        void Dwin::write_vp_command_()
        {
            uint8_t command_data[8] = {0};
            command_data[0] = 0x5a;
            command_data[1] = 0xa5;
            command_data[2] = 0x05;
            command_data[3] = 0x82;
            command_data[4] = 0x10;
            command_data[5] = 0x02;
            command_data[6] = 0x00;
            command_data[7] = 0x11;
            ESP_LOGV(TAG, "Writing to UART %d", command_data);
            this->write_array(command_data, 8);
        }

        void Dwin::write_vp_command_(uint8_t vp, uint8_t value)
        {
            uint8_t command_data[8] = {0};
            command_data[0] = 0x5a;
            command_data[1] = 0xa5;
            command_data[2] = 0x05;
            command_data[3] = 0x82;
            command_data[4] = 0x10;
            command_data[5] = vp;
            command_data[6] = 0x00;
            command_data[7] = value;
            ESP_LOGV(TAG, "Writing to UART %d", command_data);
            this->write_array(command_data, 8);
        }

    
        void Dwin::parse_data_()
        {
            this->status_clear_warning();
        }

        uint16_t Dwin::get_16_bit_uint_(uint8_t start_index) const
        {
            return (uint16_t(this->data_[start_index + 1]) << 8) | uint16_t(this->data_[start_index]);
        }

        void Dwin::dump_config()
        {
            ESP_LOGCONFIG(TAG, "DWIN:");
        }

        void Dwin::beepHMI()
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x05, CMD_WRITE, 0x00, 0xA0, 0x00, 0x7D};
            this->write_array(command_data, sizeof(command_data));
        }

        double Dwin::getHWVersion()
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_READ, 0x00, 0x0F, 0x01};
            this->write_array(command_data, sizeof(command_data));
        }

        void Dwin::setVP(unsigned short int address, byte data)
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x05 , CMD_WRITE, (address >> 8) & 0xFF, (address) & 0xFF, 0x00, data};
            this->write_array(command_data, sizeof(command_data));
        }

        void Dwin::restartHMI()
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x07, CMD_WRITE, 0x00, 0x04, 0x55, 0xaa, CMD_HEAD1, CMD_HEAD2};
            this->write_array(command_data, sizeof(command_data));
        }

        void Dwin::setBrightness(byte brigtness)
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_WRITE, 0x00, 0x82, brigtness};
            this->write_array(command_data, sizeof(command_data));
        }

        byte Dwin::getBrightness()
        {
            uint8_t command_data[] = {CMD_HEAD1, CMD_HEAD2, 0x04, CMD_READ, 0x00, 0x31, 0x01};
            this->write_array(command_data, sizeof(command_data));
        }

    } // namespace dwin
} // namespace esphome
