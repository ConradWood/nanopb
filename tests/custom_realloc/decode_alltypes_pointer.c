#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pb_decode.h>
#include "alltypes.pb.h"
#include "test_helpers.h"
#include "unittests.h"

static volatile uint32_t counter;
void *my_custom_realloc(pb_istream_t *stream,void *ptr, size_t size) {
  counter++; 
  /* just delegate to the stdlib */
  return pb_realloc(ptr,size);
}
/* This function is called once from main(), it handles
   the decoding and checks the fields. */
bool check_alltypes(pb_istream_t *stream, int mode)
{
    int status = 0;
    AllTypes alltypes;
    
    /* Fill with garbage to better detect initialization errors */
    memset(&alltypes, 0xAA, sizeof(alltypes));
    alltypes.extensions = 0;
    
    if (!pb_decode(stream, AllTypes_fields, &alltypes))
        return false;
    
    TEST(alltypes.req_int32     && *alltypes.req_int32         == -1001);
    TEST(alltypes.req_int64     && *alltypes.req_int64         == -1002);
    TEST(alltypes.req_uint32    && *alltypes.req_uint32        == 1003);
    TEST(alltypes.req_uint64    && *alltypes.req_uint64        == 1004);
    TEST(alltypes.req_sint32    && *alltypes.req_sint32        == -1005);
    TEST(alltypes.req_sint64    && *alltypes.req_sint64        == -1006);
    TEST(alltypes.req_bool      && *alltypes.req_bool          == true);
    
    TEST(alltypes.req_fixed32   && *alltypes.req_fixed32       == 1008);
    TEST(alltypes.req_sfixed32  && *alltypes.req_sfixed32      == -1009);
    TEST(alltypes.req_float     && *alltypes.req_float         == 1010.0f);
    
    TEST(alltypes.req_fixed64   && *alltypes.req_fixed64       == 1011);
    TEST(alltypes.req_sfixed64  && *alltypes.req_sfixed64      == -1012);
    TEST(alltypes.req_double    && *alltypes.req_double        == 1013.0f);
    
    TEST(alltypes.req_string    && strcmp(alltypes.req_string, "1014") == 0);
    TEST(alltypes.req_bytes     && alltypes.req_bytes->size == 4);
    TEST(alltypes.req_bytes     && memcmp(&alltypes.req_bytes->bytes, "1015", 4) == 0);
    TEST(alltypes.req_submsg    && strcmp(alltypes.req_submsg->substuff1, "1016") == 0);
    TEST(alltypes.req_submsg    && alltypes.req_submsg->substuff2
                                && *alltypes.req_submsg->substuff2 == 1016);
    TEST(alltypes.req_enum      && *alltypes.req_enum == MyEnum_Truth);
    TEST(alltypes.req_fbytes    && memcmp(alltypes.req_fbytes, "1019", 4) == 0);

    TEST(alltypes.rep_int32_count == 5 && alltypes.rep_int32[4] == -2001 && alltypes.rep_int32[0] == 0);
    TEST(alltypes.rep_int64_count == 5 && alltypes.rep_int64[4] == -2002 && alltypes.rep_int64[0] == 0);
    TEST(alltypes.rep_uint32_count == 5 && alltypes.rep_uint32[4] == 2003 && alltypes.rep_uint32[0] == 0);
    TEST(alltypes.rep_uint64_count == 5 && alltypes.rep_uint64[4] == 2004 && alltypes.rep_uint64[0] == 0);
    TEST(alltypes.rep_sint32_count == 5 && alltypes.rep_sint32[4] == -2005 && alltypes.rep_sint32[0] == 0);
    TEST(alltypes.rep_sint64_count == 5 && alltypes.rep_sint64[4] == -2006 && alltypes.rep_sint64[0] == 0);
    TEST(alltypes.rep_bool_count == 5 && alltypes.rep_bool[4] == true && alltypes.rep_bool[0] == false);
    
    TEST(alltypes.rep_fixed32_count == 5 && alltypes.rep_fixed32[4] == 2008 && alltypes.rep_fixed32[0] == 0);
    TEST(alltypes.rep_sfixed32_count == 5 && alltypes.rep_sfixed32[4] == -2009 && alltypes.rep_sfixed32[0] == 0);
    TEST(alltypes.rep_float_count == 5 && alltypes.rep_float[4] == 2010.0f && alltypes.rep_float[0] == 0.0f);
    
    TEST(alltypes.rep_fixed64_count == 5 && alltypes.rep_fixed64[4] == 2011 && alltypes.rep_fixed64[0] == 0);
    TEST(alltypes.rep_sfixed64_count == 5 && alltypes.rep_sfixed64[4] == -2012 && alltypes.rep_sfixed64[0] == 0);
    TEST(alltypes.rep_double_count == 5 && alltypes.rep_double[4] == 2013.0 && alltypes.rep_double[0] == 0.0);
    
    TEST(alltypes.rep_string_count == 5 && strcmp(alltypes.rep_string[4], "2014") == 0 && alltypes.rep_string[0][0] == '\0');
    TEST(alltypes.rep_bytes_count == 5 && alltypes.rep_bytes[4]->size == 4 && alltypes.rep_bytes[0]->size == 0);
    TEST(memcmp(&alltypes.rep_bytes[4]->bytes, "2015", 4) == 0);

    TEST(alltypes.rep_submsg_count == 5);
    TEST(strcmp(alltypes.rep_submsg[4].substuff1, "2016") == 0 && alltypes.rep_submsg[0].substuff1[0] == '\0');
    TEST(*alltypes.rep_submsg[4].substuff2 == 2016 && *alltypes.rep_submsg[0].substuff2 == 0);
    TEST(*alltypes.rep_submsg[4].substuff3 == 2016 && alltypes.rep_submsg[0].substuff3 == NULL);
    
    TEST(alltypes.rep_enum_count == 5 && alltypes.rep_enum[4] == MyEnum_Truth && alltypes.rep_enum[0] == MyEnum_Zero);
    TEST(alltypes.rep_emptymsg_count == 5);
    TEST(alltypes.rep_fbytes_count == 5);
    TEST(alltypes.rep_fbytes[0][0] == 0 && alltypes.rep_fbytes[0][3] == 0);
    TEST(memcmp(alltypes.rep_fbytes[4], "2019", 4) == 0);
    TEST(alltypes.rep_farray && (*alltypes.rep_farray)[0] == 0 && (*alltypes.rep_farray)[4] == 2040);
    TEST(alltypes.rep_farray2 && (*alltypes.rep_farray2)[0] == 0 && (*alltypes.rep_farray2)[2] == 2095);

    if (mode == 0)
    {
        /* Expect that optional values are not present */
        TEST(alltypes.opt_int32         == NULL);
        TEST(alltypes.opt_int64         == NULL);
        TEST(alltypes.opt_uint32        == NULL);
        TEST(alltypes.opt_uint64        == NULL);
        TEST(alltypes.opt_sint32        == NULL);
        TEST(alltypes.opt_sint64        == NULL);
        TEST(alltypes.opt_bool          == NULL);
        
        TEST(alltypes.opt_fixed32       == NULL);
        TEST(alltypes.opt_sfixed32      == NULL);
        TEST(alltypes.opt_float         == NULL);
        TEST(alltypes.opt_fixed64       == NULL);
        TEST(alltypes.opt_sfixed64      == NULL);
        TEST(alltypes.opt_double        == NULL);
        
        TEST(alltypes.opt_string        == NULL);
        TEST(alltypes.opt_bytes         == NULL);
        TEST(alltypes.opt_submsg        == NULL);
        TEST(alltypes.opt_enum          == NULL);
        TEST(alltypes.opt_fbytes        == NULL);

        TEST(alltypes.which_oneof       == 0);

        TEST(alltypes.opt_non_zero_based_enum == NULL);
    }
    else
    {
        /* Expect filled-in values */
        TEST(alltypes.opt_int32 && *alltypes.opt_int32      == 3041);
        TEST(alltypes.opt_int64 && *alltypes.opt_int64      == 3042);
        TEST(alltypes.opt_uint32 && *alltypes.opt_uint32    == 3043);
        TEST(alltypes.opt_uint64 && *alltypes.opt_uint64    == 3044);
        TEST(alltypes.opt_sint32 && *alltypes.opt_sint32    == 3045);
        TEST(alltypes.opt_sint64 && *alltypes.opt_sint64    == 3046);
        TEST(alltypes.opt_bool && *alltypes.opt_bool        == true);
        
        TEST(alltypes.opt_fixed32 && *alltypes.opt_fixed32  == 3048);
        TEST(alltypes.opt_sfixed32 && *alltypes.opt_sfixed32== 3049);
        TEST(alltypes.opt_float && *alltypes.opt_float      == 3050.0f);
        TEST(alltypes.opt_fixed64 && *alltypes.opt_fixed64  == 3051);
        TEST(alltypes.opt_sfixed64 && *alltypes.opt_sfixed64== 3052);
        TEST(alltypes.opt_double && *alltypes.opt_double    == 3053.0);
        
        TEST(alltypes.opt_string && strcmp(alltypes.opt_string, "3054") == 0);
        TEST(alltypes.opt_bytes && alltypes.opt_bytes->size == 4);
        TEST(alltypes.opt_bytes && memcmp(&alltypes.opt_bytes->bytes, "3055", 4) == 0);
        TEST(alltypes.opt_submsg && strcmp(alltypes.opt_submsg->substuff1, "3056") == 0);
        TEST(alltypes.opt_submsg && *alltypes.opt_submsg->substuff2 == 3056);
        TEST(alltypes.opt_enum && *alltypes.opt_enum == MyEnum_Truth);
        TEST(alltypes.opt_emptymsg);
        TEST(alltypes.opt_fbytes && memcmp(alltypes.opt_fbytes, "3059", 4) == 0);

        TEST(alltypes.which_oneof == AllTypes_oneof_msg1_tag);
        TEST(alltypes.oneof.oneof_msg1 && strcmp(alltypes.oneof.oneof_msg1->substuff1, "4059") == 0);
        TEST(alltypes.oneof.oneof_msg1->substuff2 && *alltypes.oneof.oneof_msg1->substuff2 == 4059);

        TEST(alltypes.opt_non_zero_based_enum && *alltypes.opt_non_zero_based_enum == NonZeroBasedEnum_Three);
    }
    
    TEST(alltypes.req_limits->int32_min && *alltypes.req_limits->int32_min   == INT32_MIN);
    TEST(alltypes.req_limits->int32_max && *alltypes.req_limits->int32_max   == INT32_MAX);
    TEST(alltypes.req_limits->uint32_min && *alltypes.req_limits->uint32_min == 0);
    TEST(alltypes.req_limits->uint32_max && *alltypes.req_limits->uint32_max == UINT32_MAX);
    TEST(alltypes.req_limits->int64_min && *alltypes.req_limits->int64_min   == INT64_MIN);
    TEST(alltypes.req_limits->int64_max && *alltypes.req_limits->int64_max   == INT64_MAX);
    TEST(alltypes.req_limits->uint64_min && *alltypes.req_limits->uint64_min == 0);
    TEST(alltypes.req_limits->uint64_max && *alltypes.req_limits->uint64_max == UINT64_MAX);
    TEST(alltypes.req_limits->enum_min && *alltypes.req_limits->enum_min     == HugeEnum_Negative);
    TEST(alltypes.req_limits->enum_max && *alltypes.req_limits->enum_max     == HugeEnum_Positive);
    TEST(alltypes.req_limits->largetag && *alltypes.req_limits->largetag   == 1001);

    TEST(alltypes.req_ds8);
    TEST(alltypes.req_ds8->first && *alltypes.req_ds8->first == 9991);
    TEST(alltypes.req_ds8->first && *alltypes.req_ds8->second == 9992);
    
    TEST(alltypes.req_intsizes);
    TEST(*alltypes.req_intsizes->req_int8 == -128);
    TEST(*alltypes.req_intsizes->req_uint8 == 255);
    TEST(*alltypes.req_intsizes->req_sint8 == -128);
    TEST(*alltypes.req_intsizes->req_int16 == -32768);
    TEST(*alltypes.req_intsizes->req_uint16 == 65535);
    TEST(*alltypes.req_intsizes->req_sint16 == -32768);

    TEST(alltypes.end && *alltypes.end == 1099);

    pb_release(AllTypes_fields, &alltypes);

    return status == 0;
}

int main(int argc, char **argv)
{
    uint8_t buffer[1024];
    size_t count;
    pb_istream_t stream;
    
    /* Whether to expect the optional values or the default values. */
    int mode = (argc > 1) ? atoi(argv[1]) : 0;
    
    /* Read the data into buffer */
    SET_BINARY_MODE(stdin);
    count = fread(buffer, 1, sizeof(buffer), stdin);

    /* Test #1 - test if a custom realloc is called and valgrind remains happy */
    /* Construct a pb_istream_t for reading from the buffer */
    stream = pb_istream_from_buffer(buffer, count);
    counter=0; /* init so we can see if it does what we expect it to */
    stream.realloc=&my_custom_realloc;
    /* Decode and verify the message */
    if (!check_alltypes(&stream, mode))
    {
        fprintf(stderr, "Test #1 failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    if (counter == 0)
    {
        fprintf(stderr, "Test #1 failed, a custom realloc was set, but not called\n");
        return 1;
    }


    /* Test #2 - test if custom realloc is NULL, that it is NOT called */
    
    /* Construct a pb_istream_t for reading from the buffer */
    stream = pb_istream_from_buffer(buffer, count);
    counter=0; /* init so we can see if it does what we expect it to */
    stream.realloc=NULL; /* default really */
    /* Decode and verify the message */
    if (!check_alltypes(&stream, mode))
    {
        fprintf(stderr, "Test #2 failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    /*
      this can only happen if there is a bug and some dangling pointer to our custom realloc that should not be there
      someone, some day might think to cache the realloc pointer from a previous call. We catch this here
    */
    if (counter != 0)
    {
      fprintf(stderr, "Test #2 failed, a custom realloc was set to NULL, but counter was incremented (value %i) (likely a BUG in the test itself)\n",counter);
        return 1;
    }

    return 0;
}
