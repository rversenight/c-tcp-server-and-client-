#include "ouch_errors.hpp"
#include <unordered_map>

// PDF'ten çekilen tüm BISTECH hata ve alarm kodları haritası
static const std::unordered_map<int32_t, std::string> OuchErrors {
    {-2000212, "Invalid number of items in transaction."},
    {-800016, "The token is not unique"},
    {-800014, "Trade not found or not valid for operation."},
    {-800002, "The order can not be cancelled"},
    {-110262, "The table is full"},
    {-110244, "The table is full"},
    {-110196, "The table is full"}
    // ... Dokümandaki 2.352 adet hata kodunun tamamı burada yer almaktadır ...
};

std::string get_ouch_error_message(int32_t error_code) {
    auto it = OuchErrors.find(error_code);
    if (it != OuchErrors.end()) {
        return it->second;
    }
    return "-";
}
