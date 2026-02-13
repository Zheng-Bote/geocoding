add_executable(test_reverse_geocode
    test_reverse_geocode.cpp
)

target_link_libraries(test_reverse_geocode
    PRIVATE
        regeocode
)

add_test(NAME reverse_geocode_test COMMAND test_reverse_geocode)
