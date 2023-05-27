// base/word.cpp

#include "ttl_define.h"

#include "common.h"
#include "base/base_exception.h"
#include "utility.h"

#include "base/word.h"

using namespace BMX2WAV;


namespace {
  const char* WORD_TABLE[] = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08",
    "09", "0A", "0B", "0C", "0D", "0E", "0F", "0G", "0H",
    "0I", "0J", "0K", "0L", "0M", "0N", "0O", "0P", "0Q",
    "0R", "0S", "0T", "0U", "0V", "0W", "0X", "0Y", "0Z",
    "10", "11", "12", "13", "14", "15", "16", "17", "18",
    "19", "1A", "1B", "1C", "1D", "1E", "1F", "1G", "1H",
    "1I", "1J", "1K", "1L", "1M", "1N", "1O", "1P", "1Q",
    "1R", "1S", "1T", "1U", "1V", "1W", "1X", "1Y", "1Z",
    "20", "21", "22", "23", "24", "25", "26", "27", "28",
    "29", "2A", "2B", "2C", "2D", "2E", "2F", "2G", "2H",
    "2I", "2J", "2K", "2L", "2M", "2N", "2O", "2P", "2Q",
    "2R", "2S", "2T", "2U", "2V", "2W", "2X", "2Y", "2Z",
    "30", "31", "32", "33", "34", "35", "36", "37", "38",
    "39", "3A", "3B", "3C", "3D", "3E", "3F", "3G", "3H",
    "3I", "3J", "3K", "3L", "3M", "3N", "3O", "3P", "3Q",
    "3R", "3S", "3T", "3U", "3V", "3W", "3X", "3Y", "3Z",
    "40", "41", "42", "43", "44", "45", "46", "47", "48",
    "49", "4A", "4B", "4C", "4D", "4E", "4F", "4G", "4H",
    "4I", "4J", "4K", "4L", "4M", "4N", "4O", "4P", "4Q",
    "4R", "4S", "4T", "4U", "4V", "4W", "4X", "4Y", "4Z",
    "50", "51", "52", "53", "54", "55", "56", "57", "58",
    "59", "5A", "5B", "5C", "5D", "5E", "5F", "5G", "5H",
    "5I", "5J", "5K", "5L", "5M", "5N", "5O", "5P", "5Q",
    "5R", "5S", "5T", "5U", "5V", "5W", "5X", "5Y", "5Z",
    "60", "61", "62", "63", "64", "65", "66", "67", "68",
    "69", "6A", "6B", "6C", "6D", "6E", "6F", "6G", "6H",
    "6I", "6J", "6K", "6L", "6M", "6N", "6O", "6P", "6Q",
    "6R", "6S", "6T", "6U", "6V", "6W", "6X", "6Y", "6Z",
    "70", "71", "72", "73", "74", "75", "76", "77", "78",
    "79", "7A", "7B", "7C", "7D", "7E", "7F", "7G", "7H",
    "7I", "7J", "7K", "7L", "7M", "7N", "7O", "7P", "7Q",
    "7R", "7S", "7T", "7U", "7V", "7W", "7X", "7Y", "7Z",
    "80", "81", "82", "83", "84", "85", "86", "87", "88",
    "89", "8A", "8B", "8C", "8D", "8E", "8F", "8G", "8H",
    "8I", "8J", "8K", "8L", "8M", "8N", "8O", "8P", "8Q",
    "8R", "8S", "8T", "8U", "8V", "8W", "8X", "8Y", "8Z",
    "90", "91", "92", "93", "94", "95", "96", "97", "98",
    "99", "9A", "9B", "9C", "9D", "9E", "9F", "9G", "9H",
    "9I", "9J", "9K", "9L", "9M", "9N", "9O", "9P", "9Q",
    "9R", "9S", "9T", "9U", "9V", "9W", "9X", "9Y", "9Z",
    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8",
    "A9", "AA", "AB", "AC", "AD", "AE", "AF", "AG", "AH",
    "AI", "AJ", "AK", "AL", "AM", "AN", "AO", "AP", "AQ",
    "AR", "AS", "AT", "AU", "AV", "AW", "AX", "AY", "AZ",
    "B0", "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8",
    "B9", "BA", "BB", "BC", "BD", "BE", "BF", "BG", "BH",
    "BI", "BJ", "BK", "BL", "BM", "BN", "BO", "BP", "BQ",
    "BR", "BS", "BT", "BU", "BV", "BW", "BX", "BY", "BZ",
    "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8",
    "C9", "CA", "CB", "CC", "CD", "CE", "CF", "CG", "CH",
    "CI", "CJ", "CK", "CL", "CM", "CN", "CO", "CP", "CQ",
    "CR", "CS", "CT", "CU", "CV", "CW", "CX", "CY", "CZ",
    "D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8",
    "D9", "DA", "DB", "DC", "DD", "DE", "DF", "DG", "DH",
    "DI", "DJ", "DK", "DL", "DM", "DN", "DO", "DP", "DQ",
    "DR", "DS", "DT", "DU", "DV", "DW", "DX", "DY", "DZ",
    "E0", "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8",
    "E9", "EA", "EB", "EC", "ED", "EE", "EF", "EG", "EH",
    "EI", "EJ", "EK", "EL", "EM", "EN", "EO", "EP", "EQ",
    "ER", "ES", "ET", "EU", "EV", "EW", "EX", "EY", "EZ",
    "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8",
    "F9", "FA", "FB", "FC", "FD", "FE", "FF", "FG", "FH",
    "FI", "FJ", "FK", "FL", "FM", "FN", "FO", "FP", "FQ",
    "FR", "FS", "FT", "FU", "FV", "FW", "FX", "FY", "FZ",
    "G0", "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8",
    "G9", "GA", "GB", "GC", "GD", "GE", "GF", "GG", "GH",
    "GI", "GJ", "GK", "GL", "GM", "GN", "GO", "GP", "GQ",
    "GR", "GS", "GT", "GU", "GV", "GW", "GX", "GY", "GZ",
    "H0", "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8",
    "H9", "HA", "HB", "HC", "HD", "HE", "HF", "HG", "HH",
    "HI", "HJ", "HK", "HL", "HM", "HN", "HO", "HP", "HQ",
    "HR", "HS", "HT", "HU", "HV", "HW", "HX", "HY", "HZ",
    "I0", "I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8",
    "I9", "IA", "IB", "IC", "ID", "IE", "IF", "IG", "IH",
    "II", "IJ", "IK", "IL", "IM", "IN", "IO", "IP", "IQ",
    "IR", "IS", "IT", "IU", "IV", "IW", "IX", "IY", "IZ",
    "J0", "J1", "J2", "J3", "J4", "J5", "J6", "J7", "J8",
    "J9", "JA", "JB", "JC", "JD", "JE", "JF", "JG", "JH",
    "JI", "JJ", "JK", "JL", "JM", "JN", "JO", "JP", "JQ",
    "JR", "JS", "JT", "JU", "JV", "JW", "JX", "JY", "JZ",
    "K0", "K1", "K2", "K3", "K4", "K5", "K6", "K7", "K8",
    "K9", "KA", "KB", "KC", "KD", "KE", "KF", "KG", "KH",
    "KI", "KJ", "KK", "KL", "KM", "KN", "KO", "KP", "KQ",
    "KR", "KS", "KT", "KU", "KV", "KW", "KX", "KY", "KZ",
    "L0", "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8",
    "L9", "LA", "LB", "LC", "LD", "LE", "LF", "LG", "LH",
    "LI", "LJ", "LK", "LL", "LM", "LN", "LO", "LP", "LQ",
    "LR", "LS", "LT", "LU", "LV", "LW", "LX", "LY", "LZ",
    "M0", "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8",
    "M9", "MA", "MB", "MC", "MD", "ME", "MF", "MG", "MH",
    "MI", "MJ", "MK", "ML", "MM", "MN", "MO", "MP", "MQ",
    "MR", "MS", "MT", "MU", "MV", "MW", "MX", "MY", "MZ",
    "N0", "N1", "N2", "N3", "N4", "N5", "N6", "N7", "N8",
    "N9", "NA", "NB", "NC", "ND", "NE", "NF", "NG", "NH",
    "NI", "NJ", "NK", "NL", "NM", "NN", "NO", "NP", "NQ",
    "NR", "NS", "NT", "NU", "NV", "NW", "NX", "NY", "NZ",
    "O0", "O1", "O2", "O3", "O4", "O5", "O6", "O7", "O8",
    "O9", "OA", "OB", "OC", "OD", "OE", "OF", "OG", "OH",
    "OI", "OJ", "OK", "OL", "OM", "ON", "OO", "OP", "OQ",
    "OR", "OS", "OT", "OU", "OV", "OW", "OX", "OY", "OZ",
    "P0", "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8",
    "P9", "PA", "PB", "PC", "PD", "PE", "PF", "PG", "PH",
    "PI", "PJ", "PK", "PL", "PM", "PN", "PO", "PP", "PQ",
    "PR", "PS", "PT", "PU", "PV", "PW", "PX", "PY", "PZ",
    "Q0", "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7", "Q8",
    "Q9", "QA", "QB", "QC", "QD", "QE", "QF", "QG", "QH",
    "QI", "QJ", "QK", "QL", "QM", "QN", "QO", "QP", "QQ",
    "QR", "QS", "QT", "QU", "QV", "QW", "QX", "QY", "QZ",
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8",
    "R9", "RA", "RB", "RC", "RD", "RE", "RF", "RG", "RH",
    "RI", "RJ", "RK", "RL", "RM", "RN", "RO", "RP", "RQ",
    "RR", "RS", "RT", "RU", "RV", "RW", "RX", "RY", "RZ",
    "S0", "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8",
    "S9", "SA", "SB", "SC", "SD", "SE", "SF", "SG", "SH",
    "SI", "SJ", "SK", "SL", "SM", "SN", "SO", "SP", "SQ",
    "SR", "SS", "ST", "SU", "SV", "SW", "SX", "SY", "SZ",
    "T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8",
    "T9", "TA", "TB", "TC", "TD", "TE", "TF", "TG", "TH",
    "TI", "TJ", "TK", "TL", "TM", "TN", "TO", "TP", "TQ",
    "TR", "TS", "TT", "TU", "TV", "TW", "TX", "TY", "TZ",
    "U0", "U1", "U2", "U3", "U4", "U5", "U6", "U7", "U8",
    "U9", "UA", "UB", "UC", "UD", "UE", "UF", "UG", "UH",
    "UI", "UJ", "UK", "UL", "UM", "UN", "UO", "UP", "UQ",
    "UR", "US", "UT", "UU", "UV", "UW", "UX", "UY", "UZ",
    "V0", "V1", "V2", "V3", "V4", "V5", "V6", "V7", "V8",
    "V9", "VA", "VB", "VC", "VD", "VE", "VF", "VG", "VH",
    "VI", "VJ", "VK", "VL", "VM", "VN", "VO", "VP", "VQ",
    "VR", "VS", "VT", "VU", "VV", "VW", "VX", "VY", "VZ",
    "W0", "W1", "W2", "W3", "W4", "W5", "W6", "W7", "W8",
    "W9", "WA", "WB", "WC", "WD", "WE", "WF", "WG", "WH",
    "WI", "WJ", "WK", "WL", "WM", "WN", "WO", "WP", "WQ",
    "WR", "WS", "WT", "WU", "WV", "WW", "WX", "WY", "WZ",
    "X0", "X1", "X2", "X3", "X4", "X5", "X6", "X7", "X8",
    "X9", "XA", "XB", "XC", "XD", "XE", "XF", "XG", "XH",
    "XI", "XJ", "XK", "XL", "XM", "XN", "XO", "XP", "XQ",
    "XR", "XS", "XT", "XU", "XV", "XW", "XX", "XY", "XZ",
    "Y0", "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", "Y8",
    "Y9", "YA", "YB", "YC", "YD", "YE", "YF", "YG", "YH",
    "YI", "YJ", "YK", "YL", "YM", "YN", "YO", "YP", "YQ",
    "YR", "YS", "YT", "YU", "YV", "YW", "YX", "YY", "YZ",
    "Z0", "Z1", "Z2", "Z3", "Z4", "Z5", "Z6", "Z7", "Z8",
    "Z9", "ZA", "ZB", "ZC", "ZD", "ZE", "ZF", "ZG", "ZH",
    "ZI", "ZJ", "ZK", "ZL", "ZM", "ZN", "ZO", "ZP", "ZQ",
    "ZR", "ZS", "ZT", "ZU", "ZV", "ZW", "ZX", "ZY", "ZZ",
  };
}

// -- BL::Word -----------------------------------------------------------
const BL::Word BL::Word::MIN( Const::WORD_MIN_VALUE );
const BL::Word BL::Word::MAX( Const::WORD_MAX_VALUE );

bool
BL::Word::CanConstructAsWord( const char* str )
{
  return Utility::IsHex36( str[0] ) && Utility::IsHex36( str[1] );
}

bool
BL::Word::CanConstructAsWord( const std::string& str )
{
  return Word::CanConstructAsWord( str.c_str() );
}


BL::Word::Word( void ) :
value_( 0 )
{
}


BL::Word::Word( int value ) :
value_( value )
{
}

BL::Word::Word( char msb, char lsb ) :
value_( 0 )
{
  if ( Utility::IsNotHex36( msb ) || Utility::IsNotHex36( lsb ) ) {
    throw BL::InvalidCharUsedAsWordException( msb, lsb );
  }
  value_ = Utility::Hex36ToInteger( msb ) * 36 + Utility::Hex36ToInteger( lsb );
}

BL::Word::Word( const char* str ) :
BL::Word( str[0], str[1] )
{
}

BL::Word::Word( const std::string& str ) :
BL::Word( str.c_str()[0], str.c_str()[1] )
{
}


BL::Word::operator int() const
{
  return value_;
}


void
BL::Word::Increase( void )
{
  ++value_;
}


int
BL::Word::ToInteger( void ) const
{
  return value_;
}

char
BL::Word::GetMSB( void ) const
{
  return this->ToCharPointer()[0];
}

char
BL::Word::GetLSB( void ) const
{
  return this->ToCharPointer()[1];
}

const char*
BL::Word::ToCharPointer( void ) const
{
  this->Validate();
  return WORD_TABLE[value_];
}

std::string
BL::Word::ToString( void ) const
{
  return this->ToCharPointer();
}


void
BL::Word::Validate( void ) const
{
  if ( value_ < Const::WORD_MIN_VALUE || value_ > Const::WORD_MAX_VALUE ) {
    throw BL::InvalidWordValueUsedException( value_ );
  }
}


std::ostream&
operator <<( std::ostream& os, const BL::Word& word )
{
  os << word.ToCharPointer();
  return os;
}


BL::Word
BL::operator "" _hex36( const char* str, std::size_t size )
{
  NOT_USE( size );
  return BL::Word( str );
}

// -- ObjectWithLocation -------------------------------------------------
BL::ObjectWithLocation::ObjectWithLocation( unsigned int bar_number, BL::Word channel_number, BL::Word object_number ) :
bar_number_( bar_number ),
channel_number_( channel_number ),
object_number_( object_number )
{
}
