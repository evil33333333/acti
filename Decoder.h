#pragma once
#include <iostream>
#include <vector>

class Decoder {
public:
	static std::string DecodeSystemProductKey(std::vector<unsigned char>);
	static std::vector<unsigned char> GetDigitalProductId();
};