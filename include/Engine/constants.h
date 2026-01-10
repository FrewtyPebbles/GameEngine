#pragma once

#include <array>
#include <string>

#define ENGINE_NAME "Tritium"

#define ENGINE_VERSION_MAJOR 0
#define ENGINE_VERSION_MINOR 0
#define ENGINE_VERSION_PATCH 0
#define ENGINE_VERSION_IDENTIFIER "dev"

constexpr std::string GET_ENGINE_VERSION();

// HELPER FUNCTIONS
namespace {
    // namespace makes them private to this file (kinda).

    constexpr std::array<char, 12> int_to_c_arr(int n) {
        // Handle the sign and absolute value
        int temp = n < 0 ? -n : n;
        bool is_negative = n < 0;

        // We can support up to a certain number of digits at compile time
        // Max size for 32-bit int is 10 digits + sign + null terminator
        std::array<char, 12> buffer{};
        int i = 0;

        // Handle 0 specifically
        if (temp == 0) {
            buffer[i++] = '0';
        }
        else {
            // Convert digits in reverse order
            while (temp != 0) {
                buffer[i++] = static_cast<char>((temp % 10) + '0');
                temp /= 10;
            }
        }

        // Add sign if necessary
        if (is_negative) {
            buffer[i++] = '-';
        }

        // Reverse the digits back into correct order
        std::reverse(buffer.begin(), buffer.begin() + i);

        // Create a final, tightly-sized array to return
        std::array<char, 12> result{}; // +1 for null terminator
        std::copy(buffer.begin(), buffer.begin() + i, result.begin());

        return result;
    }

    constexpr size_t c_strlen(const char* str) {
        size_t len = 0;
        while (str[len] != '\0') {
            len++;
        }
        return len;
    }

    template<size_t N1, size_t N2>
    constexpr std::array<char, N1 + N2> cat_c_arrs(
        const std::array<char, N1>& arr1,
        const std::array<char, N2>& arr2)
    {
        // Create the destination array with the combined size (N1 + N2)
        std::array<char, N1 + N2> result{};

        // Use std::copy (which is constexpr in C++20) to copy the first array
        // 'current_pos' iterator points to the end of the newly copied elements
        auto current_pos = std::copy(arr1.begin(), arr1.end(), result.begin());

        // Copy the second array starting from that position
        std::copy(arr2.begin(), arr2.end(), current_pos);

        return result;
    }

    template<size_t N1, size_t N2>
    constexpr std::array<char, N1 + N2 + 1> cat_arr_cstr(
        const std::array<char, N1>& arr,
        const char(&cstr)[N2])
    {
        // N2 includes the null terminator of the string literal, 
        // so the total size is N1 + N2 + 1 to account for the new combined null terminator.
        std::array<char, N1 + N2 + 1> result{};

        // Copy the std::array first (C++20 std::copy is constexpr)
        auto current_pos = std::copy(arr.begin(), arr.end(), result.begin());

        // Copy the c-string, excluding its original null terminator
        std::copy(cstr, cstr + N2, current_pos);

        // Explicitly place the final null terminator
        result[N1 + N2] = '\0';

        return result;
    }
}

constexpr std::string GET_ENGINE_VERSION() {
	// Returns the engine version string.
    auto vMaj = int_to_c_arr(ENGINE_VERSION_MAJOR);
    auto vMin = int_to_c_arr(ENGINE_VERSION_MINOR);
    auto vPat = int_to_c_arr(ENGINE_VERSION_PATCH);

    auto version_arr = cat_c_arrs(cat_c_arrs(cat_arr_cstr(vMaj, "."), cat_arr_cstr(vMin, ".")), cat_arr_cstr(vPat, ENGINE_VERSION_IDENTIFIER));

    return std::string(version_arr.begin(), version_arr.end());
}