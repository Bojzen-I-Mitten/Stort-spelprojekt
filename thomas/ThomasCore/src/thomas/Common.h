#pragma once
#include <iostream>
#include <sstream>
#define LOG(msg){std::stringstream __buff__; __buff__ << msg; thomas::ThomasCore::LogOutput(__buff__.str());}
#define HR_TO_STRING(hr){std::string(_com_error(hr).ErrorMessage())}
#define LOG_HR(hr){_com_error err(hr); std::string error = std::string(err.ErrorMessage()); LOG(error);}

#define SAFE_RELEASE(p) { if (p) { p->Release(); p = nullptr; } }
#define SAFE_DELETE(p) { if (p) { delete p; p = nullptr; } }
#define THOMAS_AA_COUNT 4
#define THOMAS_AA_QUALITY 0

