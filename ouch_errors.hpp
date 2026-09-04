#ifndef OUCH_ERRORS_HPP
#define OUCH_ERRORS_HPP

#include <string>
#include <cstdint>

// BISTECH hata kodunun açıklamasını döndürür. Kod bulunamazsa "-" döner.
std::string get_ouch_error_message(int32_t error_code);

#endif // OUCH_ERRORS_HPP
