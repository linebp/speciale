#!/bin/sh

cpp <<HERE | tail -1
#include <limits.h>
ARG_MAX
HERE
