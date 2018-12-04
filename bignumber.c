/*
    2018 Matthew Darnell
*/
#include <gmp.h>
#include <string.h>
#include "redismodule.h"
/**
    BIGNUM <key> <incrby/set> <new value>
*/
int BigNumCommand(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
  if (argc != 4) { // Enforce 4 args
    return RedisModule_WrongArity(ctx);
  }
  RedisModule_AutoMemory(
      ctx); // Internal Redis call for activating auto mem management

  RedisModuleString *Key = argv[1];
  RedisModuleString *Command = argv[2];
  RedisModuleString *Value = argv[3];

  size_t command_len, value_len;
  const char *str_command = RedisModule_StringPtrLen(Command, &command_len);
  const char *str_value = RedisModule_StringPtrLen(Value, &value_len);

  if (strcasecmp(str_command, "set") != 0 &&
      strcasecmp(str_command, "incrby") != 0) { // Validate SET/INCRBY command
    return RedisModule_ReplyWithError(ctx, "Wrong Arguments");
  }

  RedisModuleString *value_to_set;
  mpz_t result;
  char buf[1024] = {0};

  if (strcasecmp(str_command, "set") == 0) { // Simply set the value passed...
    mpz_init_set_str(result, str_value, 10);
    gmp_snprintf(buf, 1024, "%Zd", result);
    mpz_clear(result);

    value_to_set = RedisModule_CreateString(ctx, buf, strlen(buf));
    RedisModuleCallReply *rep = RedisModule_Call(ctx, "SET", "ss", Key, value_to_set); // Set the new value

    if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
      RedisModule_ReplyWithCallReply(ctx, rep);
      return REDISMODULE_ERR;
    }
    return RedisModule_ReplyWithCallReply(ctx, rep);
  } else { //...Otherwise we just add the passed value
    RedisModuleCallReply *get_reply = RedisModule_Call(ctx, "GET", "s", Key);
    if (RedisModule_CallReplyType(get_reply) == REDISMODULE_REPLY_ERROR) { // And let’s make sure it’s not an error
      RedisModule_ReplyWithCallReply(ctx, get_reply);
      return REDISMODULE_ERR;
    }

    size_t get_reply_len;
    const char *str_rep = RedisModule_CallReplyStringPtr(get_reply, &get_reply_len);

    mpz_t x, y;
    if (str_rep == NULL) {
      mpz_init_set_str(x, "0", 10);
    } else {
      mpz_init_set_str(x, str_rep, 10);
    }

    mpz_init_set_str(y, str_value, 10);
    mpz_init(result);
    mpz_add(result, x, y);
    gmp_snprintf(buf, 1024, "%Zd", result);
    value_to_set = RedisModule_CreateString(ctx, buf, strlen(buf));
    mpz_clear(x);
    mpz_clear(y);
    mpz_clear(result);
  }

  RedisModuleCallReply *rep = RedisModule_Call(ctx, "SET", "ss", Key, value_to_set); // Set the new value
  // And let’s make sure it’s not an error
  if (RedisModule_CallReplyType(rep) == REDISMODULE_REPLY_ERROR) {
    RedisModule_ReplyWithCallReply(ctx, rep);
    return REDISMODULE_ERR;
  }
  return RedisModule_ReplyWithCallReply(ctx, rep);
}
int RedisModule_OnLoad(RedisModuleCtx *ctx) {
  if (RedisModule_Init(ctx, "bignum", 1, REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }
  if (RedisModule_CreateCommand(ctx, "bignum", BigNumCommand, "write", 1, 1, 1) == REDISMODULE_ERR) {
    return REDISMODULE_ERR;
  }
  return REDISMODULE_OK;
}
