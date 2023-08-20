#pragma once
#pragma warning(disable : 4996)

#include <SDKDDKVer.h>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/json.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/beast/core/detail/base64.hpp>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <iostream>
#include <Windows.h>
#include <d3d9.h>
#include <thread>
#include <future>
#include <atomic>
#include <iomanip>
#include <map>
#include <TlHelp32.h>
#include <winternl.h>
#include <psapi.h>
#include <string>
#include <fstream>
#include <typeinfo>
#include <memory>
#include <random>
#include <vector>
#include <type_traits>
#include <utility>

#include <detours.h>


namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
namespace ssl = net::ssl;       // from <boost/asio/ssl.hpp>
namespace json = boost::json;
//namespace pt = boost::property_tree;
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_stdlib.h"
#include "imgui/imgui_internal.h"

#include "xorStr.h"
#include "defineStrings.h"
#include "instr.h"

#include "threadCaller.h"
#include "hwidUtils.h"


#include "httpStruct.h"
#include "ntFunctionsStruct.h"
#include "restClient.h"
#include "dtoImpl.h"
#include "apiConnector.h"
#include "debuggerCheck.h"
#include "security.h"
#include "injection.h"
#include "globals.h"
#include "main.h"