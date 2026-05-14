/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
# define YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int base_yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENT = 258,                   /* IDENT  */
    FCONST = 259,                  /* FCONST  */
    SCONST = 260,                  /* SCONST  */
    BCONST = 261,                  /* BCONST  */
    XCONST = 262,                  /* XCONST  */
    Op = 263,                      /* Op  */
    ICONST = 264,                  /* ICONST  */
    PARAM = 265,                   /* PARAM  */
    TYPECAST = 266,                /* TYPECAST  */
    DOT_DOT = 267,                 /* DOT_DOT  */
    COLON_EQUALS = 268,            /* COLON_EQUALS  */
    EQUALS_GREATER = 269,          /* EQUALS_GREATER  */
    INTEGER_DIVISION = 270,        /* INTEGER_DIVISION  */
    POWER_OF = 271,                /* POWER_OF  */
    SINGLE_ARROW = 272,            /* SINGLE_ARROW  */
    DOUBLE_ARROW = 273,            /* DOUBLE_ARROW  */
    SINGLE_COLON = 274,            /* SINGLE_COLON  */
    LESS_EQUALS = 275,             /* LESS_EQUALS  */
    GREATER_EQUALS = 276,          /* GREATER_EQUALS  */
    NOT_EQUALS = 277,              /* NOT_EQUALS  */
    ABORT_P = 278,                 /* ABORT_P  */
    ABSOLUTE_P = 279,              /* ABSOLUTE_P  */
    ACCESS = 280,                  /* ACCESS  */
    ACTION = 281,                  /* ACTION  */
    ADD_P = 282,                   /* ADD_P  */
    ADMIN = 283,                   /* ADMIN  */
    AFTER = 284,                   /* AFTER  */
    AGG = 285,                     /* AGG  */
    AGGREGATE = 286,               /* AGGREGATE  */
    ALL = 287,                     /* ALL  */
    ALSO = 288,                    /* ALSO  */
    ALTER = 289,                   /* ALTER  */
    ALWAYS = 290,                  /* ALWAYS  */
    ANALYSE = 291,                 /* ANALYSE  */
    ANALYZE = 292,                 /* ANALYZE  */
    AND = 293,                     /* AND  */
    ANTI = 294,                    /* ANTI  */
    ANY = 295,                     /* ANY  */
    API = 296,                     /* API  */
    ARRAY = 297,                   /* ARRAY  */
    AS = 298,                      /* AS  */
    ASC_P = 299,                   /* ASC_P  */
    ASOF = 300,                    /* ASOF  */
    ASSERTION = 301,               /* ASSERTION  */
    ASSIGNMENT = 302,              /* ASSIGNMENT  */
    ASYMMETRIC = 303,              /* ASYMMETRIC  */
    AT = 304,                      /* AT  */
    ATTACH = 305,                  /* ATTACH  */
    ATTRIBUTE = 306,               /* ATTRIBUTE  */
    AUTHORIZATION = 307,           /* AUTHORIZATION  */
    BACKWARD = 308,                /* BACKWARD  */
    BEFORE = 309,                  /* BEFORE  */
    BEGIN_P = 310,                 /* BEGIN_P  */
    BETWEEN = 311,                 /* BETWEEN  */
    BIGINT = 312,                  /* BIGINT  */
    BINARY = 313,                  /* BINARY  */
    BIT = 314,                     /* BIT  */
    BOOLEAN_P = 315,               /* BOOLEAN_P  */
    BOTH = 316,                    /* BOTH  */
    BY = 317,                      /* BY  */
    CACHE = 318,                   /* CACHE  */
    CALL_P = 319,                  /* CALL_P  */
    CALLED = 320,                  /* CALLED  */
    CASCADE = 321,                 /* CASCADE  */
    CASCADED = 322,                /* CASCADED  */
    CASE = 323,                    /* CASE  */
    CAST = 324,                    /* CAST  */
    CATALOG_P = 325,               /* CATALOG_P  */
    CENTURIES_P = 326,             /* CENTURIES_P  */
    CENTURY_P = 327,               /* CENTURY_P  */
    CHAIN = 328,                   /* CHAIN  */
    CHAR_P = 329,                  /* CHAR_P  */
    CHARACTER = 330,               /* CHARACTER  */
    CHARACTERISTICS = 331,         /* CHARACTERISTICS  */
    CHECK_P = 332,                 /* CHECK_P  */
    CHECKPOINT = 333,              /* CHECKPOINT  */
    CLASS = 334,                   /* CLASS  */
    CLOSE = 335,                   /* CLOSE  */
    CLUSTER = 336,                 /* CLUSTER  */
    COALESCE = 337,                /* COALESCE  */
    COLLATE = 338,                 /* COLLATE  */
    COLLATION = 339,               /* COLLATION  */
    COLUMN = 340,                  /* COLUMN  */
    COLUMNS = 341,                 /* COLUMNS  */
    COMMENT = 342,                 /* COMMENT  */
    COMMENTS = 343,                /* COMMENTS  */
    COMMIT = 344,                  /* COMMIT  */
    COMMITTED = 345,               /* COMMITTED  */
    COMPRESSION = 346,             /* COMPRESSION  */
    CONCURRENTLY = 347,            /* CONCURRENTLY  */
    CONFIGURATION = 348,           /* CONFIGURATION  */
    CONFLICT = 349,                /* CONFLICT  */
    CONNECTION = 350,              /* CONNECTION  */
    CONSTRAINT = 351,              /* CONSTRAINT  */
    CONSTRAINTS = 352,             /* CONSTRAINTS  */
    CONTENT_P = 353,               /* CONTENT_P  */
    CONTINUE_P = 354,              /* CONTINUE_P  */
    CONVERSION_P = 355,            /* CONVERSION_P  */
    COPY = 356,                    /* COPY  */
    COST = 357,                    /* COST  */
    CREATE_P = 358,                /* CREATE_P  */
    CROSS = 359,                   /* CROSS  */
    CSV = 360,                     /* CSV  */
    CUBE = 361,                    /* CUBE  */
    CURRENT_P = 362,               /* CURRENT_P  */
    CURSOR = 363,                  /* CURSOR  */
    CYCLE = 364,                   /* CYCLE  */
    DATA_P = 365,                  /* DATA_P  */
    DATABASE = 366,                /* DATABASE  */
    DAY_P = 367,                   /* DAY_P  */
    DAYS_P = 368,                  /* DAYS_P  */
    DEALLOCATE = 369,              /* DEALLOCATE  */
    DEC = 370,                     /* DEC  */
    DECADE_P = 371,                /* DECADE_P  */
    DECADES_P = 372,               /* DECADES_P  */
    DECIMAL_P = 373,               /* DECIMAL_P  */
    DECLARE = 374,                 /* DECLARE  */
    DEFAULT = 375,                 /* DEFAULT  */
    DEFAULTS = 376,                /* DEFAULTS  */
    DEFERRABLE = 377,              /* DEFERRABLE  */
    DEFERRED = 378,                /* DEFERRED  */
    DEFINER = 379,                 /* DEFINER  */
    DELETE_P = 380,                /* DELETE_P  */
    DELIMITER = 381,               /* DELIMITER  */
    DELIMITERS = 382,              /* DELIMITERS  */
    DEPENDS = 383,                 /* DEPENDS  */
    DESC_P = 384,                  /* DESC_P  */
    DESCRIBE = 385,                /* DESCRIBE  */
    DETACH = 386,                  /* DETACH  */
    DICTIONARY = 387,              /* DICTIONARY  */
    DISABLE_P = 388,               /* DISABLE_P  */
    DISCARD = 389,                 /* DISCARD  */
    DISTINCT = 390,                /* DISTINCT  */
    DO = 391,                      /* DO  */
    DOCUMENT_P = 392,              /* DOCUMENT_P  */
    DOMAIN_P = 393,                /* DOMAIN_P  */
    DOUBLE_P = 394,                /* DOUBLE_P  */
    DROP = 395,                    /* DROP  */
    EACH = 396,                    /* EACH  */
    EDGES = 397,                   /* EDGES  */
    ELSE = 398,                    /* ELSE  */
    EMBED = 399,                   /* EMBED  */
    EMBEDDING = 400,               /* EMBEDDING  */
    ENABLE_P = 401,                /* ENABLE_P  */
    ENCODING = 402,                /* ENCODING  */
    ENCRYPTED = 403,               /* ENCRYPTED  */
    END_P = 404,                   /* END_P  */
    ENUM_P = 405,                  /* ENUM_P  */
    ERROR_P = 406,                 /* ERROR_P  */
    ESCAPE = 407,                  /* ESCAPE  */
    EVENT = 408,                   /* EVENT  */
    EXCEPT = 409,                  /* EXCEPT  */
    EXCLUDE = 410,                 /* EXCLUDE  */
    EXCLUDING = 411,               /* EXCLUDING  */
    EXCLUSIVE = 412,               /* EXCLUSIVE  */
    EXECUTE = 413,                 /* EXECUTE  */
    EXISTS = 414,                  /* EXISTS  */
    EXPLAIN = 415,                 /* EXPLAIN  */
    EXPORT_P = 416,                /* EXPORT_P  */
    EXPORT_STATE = 417,            /* EXPORT_STATE  */
    EXTENSION = 418,               /* EXTENSION  */
    EXTENSIONS = 419,              /* EXTENSIONS  */
    EXTERNAL = 420,                /* EXTERNAL  */
    EXTRACT = 421,                 /* EXTRACT  */
    FALSE_P = 422,                 /* FALSE_P  */
    FAMILY = 423,                  /* FAMILY  */
    FEATURES = 424,                /* FEATURES  */
    FETCH = 425,                   /* FETCH  */
    FILTER = 426,                  /* FILTER  */
    FIRST_P = 427,                 /* FIRST_P  */
    FLOAT_P = 428,                 /* FLOAT_P  */
    FOLLOWING = 429,               /* FOLLOWING  */
    FOR = 430,                     /* FOR  */
    FORCE = 431,                   /* FORCE  */
    FOREIGN = 432,                 /* FOREIGN  */
    FORWARD = 433,                 /* FORWARD  */
    FREEZE = 434,                  /* FREEZE  */
    FROM = 435,                    /* FROM  */
    FULL = 436,                    /* FULL  */
    FUNCTION = 437,                /* FUNCTION  */
    FUNCTIONS = 438,               /* FUNCTIONS  */
    GENERATED = 439,               /* GENERATED  */
    GLOB = 440,                    /* GLOB  */
    GLOBAL = 441,                  /* GLOBAL  */
    GNN = 442,                     /* GNN  */
    GRANT = 443,                   /* GRANT  */
    GRANTED = 444,                 /* GRANTED  */
    GROUP_P = 445,                 /* GROUP_P  */
    GROUPING = 446,                /* GROUPING  */
    GROUPING_ID = 447,             /* GROUPING_ID  */
    GROUPS = 448,                  /* GROUPS  */
    HANDLER = 449,                 /* HANDLER  */
    HAVING = 450,                  /* HAVING  */
    HEADER_P = 451,                /* HEADER_P  */
    HOLD = 452,                    /* HOLD  */
    HOUR_P = 453,                  /* HOUR_P  */
    HOURS_P = 454,                 /* HOURS_P  */
    IDENTITY_P = 455,              /* IDENTITY_P  */
    IF_P = 456,                    /* IF_P  */
    IGNORE_P = 457,                /* IGNORE_P  */
    ILIKE = 458,                   /* ILIKE  */
    IMMEDIATE = 459,               /* IMMEDIATE  */
    IMMUTABLE = 460,               /* IMMUTABLE  */
    IMPLICIT_P = 461,              /* IMPLICIT_P  */
    IMPORT_P = 462,                /* IMPORT_P  */
    IN_P = 463,                    /* IN_P  */
    INCLUDE_P = 464,               /* INCLUDE_P  */
    INCLUDING = 465,               /* INCLUDING  */
    INCREMENT = 466,               /* INCREMENT  */
    INDEX = 467,                   /* INDEX  */
    INDEXES = 468,                 /* INDEXES  */
    INHERIT = 469,                 /* INHERIT  */
    INHERITS = 470,                /* INHERITS  */
    INITIALLY = 471,               /* INITIALLY  */
    INLINE_P = 472,                /* INLINE_P  */
    INNER_P = 473,                 /* INNER_P  */
    INOUT = 474,                   /* INOUT  */
    INPUT_P = 475,                 /* INPUT_P  */
    INSENSITIVE = 476,             /* INSENSITIVE  */
    INSERT = 477,                  /* INSERT  */
    INSTALL = 478,                 /* INSTALL  */
    INSTEAD = 479,                 /* INSTEAD  */
    INT_P = 480,                   /* INT_P  */
    INTEGER = 481,                 /* INTEGER  */
    INTERSECT = 482,               /* INTERSECT  */
    INTERVAL = 483,                /* INTERVAL  */
    INTO = 484,                    /* INTO  */
    INVOKER = 485,                 /* INVOKER  */
    IS = 486,                      /* IS  */
    ISNULL = 487,                  /* ISNULL  */
    ISOLATION = 488,               /* ISOLATION  */
    JOIN = 489,                    /* JOIN  */
    JSON = 490,                    /* JSON  */
    KEY = 491,                     /* KEY  */
    LABEL = 492,                   /* LABEL  */
    LAMBDA = 493,                  /* LAMBDA  */
    LANGUAGE = 494,                /* LANGUAGE  */
    LARGE_P = 495,                 /* LARGE_P  */
    LAST_P = 496,                  /* LAST_P  */
    LATERAL_P = 497,               /* LATERAL_P  */
    LEADING = 498,                 /* LEADING  */
    LEAKPROOF = 499,               /* LEAKPROOF  */
    LEFT = 500,                    /* LEFT  */
    LEVEL = 501,                   /* LEVEL  */
    LIKE = 502,                    /* LIKE  */
    LIMIT = 503,                   /* LIMIT  */
    LISTEN = 504,                  /* LISTEN  */
    LLM = 505,                     /* LLM  */
    LM = 506,                      /* LM  */
    LOAD = 507,                    /* LOAD  */
    LOCAL = 508,                   /* LOCAL  */
    LOCATION = 509,                /* LOCATION  */
    LOCK_P = 510,                  /* LOCK_P  */
    LOCKED = 511,                  /* LOCKED  */
    LOGGED = 512,                  /* LOGGED  */
    MACRO = 513,                   /* MACRO  */
    MAP = 514,                     /* MAP  */
    MAPPING = 515,                 /* MAPPING  */
    MATCH = 516,                   /* MATCH  */
    MATCHED = 517,                 /* MATCHED  */
    MATERIALIZED = 518,            /* MATERIALIZED  */
    MAXVALUE = 519,                /* MAXVALUE  */
    MERGE = 520,                   /* MERGE  */
    METHOD = 521,                  /* METHOD  */
    MICROSECOND_P = 522,           /* MICROSECOND_P  */
    MICROSECONDS_P = 523,          /* MICROSECONDS_P  */
    MILLENNIA_P = 524,             /* MILLENNIA_P  */
    MILLENNIUM_P = 525,            /* MILLENNIUM_P  */
    MILLISECOND_P = 526,           /* MILLISECOND_P  */
    MILLISECONDS_P = 527,          /* MILLISECONDS_P  */
    MINUTE_P = 528,                /* MINUTE_P  */
    MINUTES_P = 529,               /* MINUTES_P  */
    MINVALUE = 530,                /* MINVALUE  */
    MODE = 531,                    /* MODE  */
    MODEL = 532,                   /* MODEL  */
    MONTH_P = 533,                 /* MONTH_P  */
    MONTHS_P = 534,                /* MONTHS_P  */
    MOVE = 535,                    /* MOVE  */
    NAME_P = 536,                  /* NAME_P  */
    NAMES = 537,                   /* NAMES  */
    NATIONAL = 538,                /* NATIONAL  */
    NATURAL = 539,                 /* NATURAL  */
    NCHAR = 540,                   /* NCHAR  */
    NEW = 541,                     /* NEW  */
    NEXT = 542,                    /* NEXT  */
    NO = 543,                      /* NO  */
    NODES = 544,                   /* NODES  */
    NONE = 545,                    /* NONE  */
    NOT = 546,                     /* NOT  */
    NOTHING = 547,                 /* NOTHING  */
    NOTIFY = 548,                  /* NOTIFY  */
    NOTNULL = 549,                 /* NOTNULL  */
    NOWAIT = 550,                  /* NOWAIT  */
    NULL_P = 551,                  /* NULL_P  */
    NULLIF = 552,                  /* NULLIF  */
    NULLS_P = 553,                 /* NULLS_P  */
    NUMERIC = 554,                 /* NUMERIC  */
    OBJECT_P = 555,                /* OBJECT_P  */
    OF = 556,                      /* OF  */
    OFF = 557,                     /* OFF  */
    OFFSET = 558,                  /* OFFSET  */
    OIDS = 559,                    /* OIDS  */
    OLD = 560,                     /* OLD  */
    ON = 561,                      /* ON  */
    ONLY = 562,                    /* ONLY  */
    OPERATOR = 563,                /* OPERATOR  */
    OPTION = 564,                  /* OPTION  */
    OPTIONS = 565,                 /* OPTIONS  */
    OR = 566,                      /* OR  */
    ORDER = 567,                   /* ORDER  */
    ORDINALITY = 568,              /* ORDINALITY  */
    OTHERS = 569,                  /* OTHERS  */
    OUT_P = 570,                   /* OUT_P  */
    OUTER_P = 571,                 /* OUTER_P  */
    OUTPUT = 572,                  /* OUTPUT  */
    OVER = 573,                    /* OVER  */
    OVERLAPS = 574,                /* OVERLAPS  */
    OVERLAY = 575,                 /* OVERLAY  */
    OVERRIDING = 576,              /* OVERRIDING  */
    OWNED = 577,                   /* OWNED  */
    OWNER = 578,                   /* OWNER  */
    PARALLEL = 579,                /* PARALLEL  */
    PARSER = 580,                  /* PARSER  */
    PARTIAL = 581,                 /* PARTIAL  */
    PARTITION = 582,               /* PARTITION  */
    PARTITIONED = 583,             /* PARTITIONED  */
    PASSING = 584,                 /* PASSING  */
    PASSWORD = 585,                /* PASSWORD  */
    PATH = 586,                    /* PATH  */
    PERCENT = 587,                 /* PERCENT  */
    PERSISTENT = 588,              /* PERSISTENT  */
    PIVOT = 589,                   /* PIVOT  */
    PIVOT_LONGER = 590,            /* PIVOT_LONGER  */
    PIVOT_WIDER = 591,             /* PIVOT_WIDER  */
    PLACING = 592,                 /* PLACING  */
    PLANS = 593,                   /* PLANS  */
    POLICY = 594,                  /* POLICY  */
    POSITION = 595,                /* POSITION  */
    POSITIONAL = 596,              /* POSITIONAL  */
    PRAGMA_P = 597,                /* PRAGMA_P  */
    PRECEDING = 598,               /* PRECEDING  */
    PRECISION = 599,               /* PRECISION  */
    PREDICT = 600,                 /* PREDICT  */
    PREPARE = 601,                 /* PREPARE  */
    PREPARED = 602,                /* PREPARED  */
    PRESERVE = 603,                /* PRESERVE  */
    PRIMARY = 604,                 /* PRIMARY  */
    PRIOR = 605,                   /* PRIOR  */
    PRIVILEGES = 606,              /* PRIVILEGES  */
    PROCEDURAL = 607,              /* PROCEDURAL  */
    PROCEDURE = 608,               /* PROCEDURE  */
    PROGRAM = 609,                 /* PROGRAM  */
    PROMPT = 610,                  /* PROMPT  */
    PUBLICATION = 611,             /* PUBLICATION  */
    QUALIFY = 612,                 /* QUALIFY  */
    QUARTER_P = 613,               /* QUARTER_P  */
    QUARTERS_P = 614,              /* QUARTERS_P  */
    QUOTE = 615,                   /* QUOTE  */
    RANGE = 616,                   /* RANGE  */
    READ_P = 617,                  /* READ_P  */
    REAL = 618,                    /* REAL  */
    REASSIGN = 619,                /* REASSIGN  */
    RECHECK = 620,                 /* RECHECK  */
    RECURSIVE = 621,               /* RECURSIVE  */
    REF = 622,                     /* REF  */
    REFERENCES = 623,              /* REFERENCES  */
    REFERENCING = 624,             /* REFERENCING  */
    REFRESH = 625,                 /* REFRESH  */
    REINDEX = 626,                 /* REINDEX  */
    RELATIVE_P = 627,              /* RELATIVE_P  */
    RELEASE = 628,                 /* RELEASE  */
    RENAME = 629,                  /* RENAME  */
    REPEATABLE = 630,              /* REPEATABLE  */
    REPLACE = 631,                 /* REPLACE  */
    REPLICA = 632,                 /* REPLICA  */
    RESET = 633,                   /* RESET  */
    RESPECT_P = 634,               /* RESPECT_P  */
    RESTART = 635,                 /* RESTART  */
    RESTRICT = 636,                /* RESTRICT  */
    RETURNING = 637,               /* RETURNING  */
    RETURNS = 638,                 /* RETURNS  */
    REVOKE = 639,                  /* REVOKE  */
    RIGHT = 640,                   /* RIGHT  */
    ROLE = 641,                    /* ROLE  */
    ROLLBACK = 642,                /* ROLLBACK  */
    ROLLUP = 643,                  /* ROLLUP  */
    ROW = 644,                     /* ROW  */
    ROWS = 645,                    /* ROWS  */
    RULE = 646,                    /* RULE  */
    SAMPLE = 647,                  /* SAMPLE  */
    SAVEPOINT = 648,               /* SAVEPOINT  */
    SCHEMA = 649,                  /* SCHEMA  */
    SCHEMAS = 650,                 /* SCHEMAS  */
    SCOPE = 651,                   /* SCOPE  */
    SCROLL = 652,                  /* SCROLL  */
    SEARCH = 653,                  /* SEARCH  */
    SECOND_P = 654,                /* SECOND_P  */
    SECONDS_P = 655,               /* SECONDS_P  */
    SECRET = 656,                  /* SECRET  */
    SECURITY = 657,                /* SECURITY  */
    SELECT = 658,                  /* SELECT  */
    SEMI = 659,                    /* SEMI  */
    SEQUENCE = 660,                /* SEQUENCE  */
    SEQUENCES = 661,               /* SEQUENCES  */
    SERIALIZABLE = 662,            /* SERIALIZABLE  */
    SERVER = 663,                  /* SERVER  */
    SESSION = 664,                 /* SESSION  */
    SET = 665,                     /* SET  */
    SETOF = 666,                   /* SETOF  */
    SETS = 667,                    /* SETS  */
    SHARE = 668,                   /* SHARE  */
    SHOW = 669,                    /* SHOW  */
    SIMILAR = 670,                 /* SIMILAR  */
    SIMPLE = 671,                  /* SIMPLE  */
    SIZE = 672,                    /* SIZE  */
    SKIP = 673,                    /* SKIP  */
    SMALLINT = 674,                /* SMALLINT  */
    SNAPSHOT = 675,                /* SNAPSHOT  */
    SOME = 676,                    /* SOME  */
    SORTED = 677,                  /* SORTED  */
    SOURCE_P = 678,                /* SOURCE_P  */
    SQL_P = 679,                   /* SQL_P  */
    STABLE = 680,                  /* STABLE  */
    STANDALONE_P = 681,            /* STANDALONE_P  */
    START = 682,                   /* START  */
    STATEMENT = 683,               /* STATEMENT  */
    STATISTICS = 684,              /* STATISTICS  */
    STDIN = 685,                   /* STDIN  */
    STDOUT = 686,                  /* STDOUT  */
    STORAGE = 687,                 /* STORAGE  */
    STORED = 688,                  /* STORED  */
    STRICT_P = 689,                /* STRICT_P  */
    STRIP_P = 690,                 /* STRIP_P  */
    STRUCT = 691,                  /* STRUCT  */
    SUBSCRIPTION = 692,            /* SUBSCRIPTION  */
    SUBSTRING = 693,               /* SUBSTRING  */
    SUMMARIZE = 694,               /* SUMMARIZE  */
    SYMMETRIC = 695,               /* SYMMETRIC  */
    SYSID = 696,                   /* SYSID  */
    SYSTEM_P = 697,                /* SYSTEM_P  */
    TABLE = 698,                   /* TABLE  */
    TABLES = 699,                  /* TABLES  */
    TABLESAMPLE = 700,             /* TABLESAMPLE  */
    TABLESPACE = 701,              /* TABLESPACE  */
    TABULAR = 702,                 /* TABULAR  */
    TAKES = 703,                   /* TAKES  */
    TARGET_P = 704,                /* TARGET_P  */
    TEMP = 705,                    /* TEMP  */
    TEMPLATE = 706,                /* TEMPLATE  */
    TEMPORARY = 707,               /* TEMPORARY  */
    TEXT_P = 708,                  /* TEXT_P  */
    THEN = 709,                    /* THEN  */
    TIES = 710,                    /* TIES  */
    TIME = 711,                    /* TIME  */
    TIMESTAMP = 712,               /* TIMESTAMP  */
    TO = 713,                      /* TO  */
    TRAILING = 714,                /* TRAILING  */
    TRANSACTION = 715,             /* TRANSACTION  */
    TRANSFORM = 716,               /* TRANSFORM  */
    TREAT = 717,                   /* TREAT  */
    TRIGGER = 718,                 /* TRIGGER  */
    TRIM = 719,                    /* TRIM  */
    TRUE_P = 720,                  /* TRUE_P  */
    TRUNCATE = 721,                /* TRUNCATE  */
    TRUSTED = 722,                 /* TRUSTED  */
    TRY_CAST = 723,                /* TRY_CAST  */
    TYPE_P = 724,                  /* TYPE_P  */
    TYPES_P = 725,                 /* TYPES_P  */
    UNBOUNDED = 726,               /* UNBOUNDED  */
    UNCOMMITTED = 727,             /* UNCOMMITTED  */
    UNENCRYPTED = 728,             /* UNENCRYPTED  */
    UNION = 729,                   /* UNION  */
    UNIQUE = 730,                  /* UNIQUE  */
    UNKNOWN = 731,                 /* UNKNOWN  */
    UNLISTEN = 732,                /* UNLISTEN  */
    UNLOGGED = 733,                /* UNLOGGED  */
    UNPACK = 734,                  /* UNPACK  */
    UNPIVOT = 735,                 /* UNPIVOT  */
    UNTIL = 736,                   /* UNTIL  */
    UPDATE = 737,                  /* UPDATE  */
    USE_P = 738,                   /* USE_P  */
    USER = 739,                    /* USER  */
    USING = 740,                   /* USING  */
    VACUUM = 741,                  /* VACUUM  */
    VALID = 742,                   /* VALID  */
    VALIDATE = 743,                /* VALIDATE  */
    VALIDATOR = 744,               /* VALIDATOR  */
    VALUE_P = 745,                 /* VALUE_P  */
    VALUES = 746,                  /* VALUES  */
    VARCHAR = 747,                 /* VARCHAR  */
    VARIABLE_P = 748,              /* VARIABLE_P  */
    VARIADIC = 749,                /* VARIADIC  */
    VARYING = 750,                 /* VARYING  */
    VERBOSE = 751,                 /* VERBOSE  */
    VERSION_P = 752,               /* VERSION_P  */
    VIEW = 753,                    /* VIEW  */
    VIEWS = 754,                   /* VIEWS  */
    VIRTUAL = 755,                 /* VIRTUAL  */
    VOLATILE = 756,                /* VOLATILE  */
    WEEK_P = 757,                  /* WEEK_P  */
    WEEKS_P = 758,                 /* WEEKS_P  */
    WHEN = 759,                    /* WHEN  */
    WHERE = 760,                   /* WHERE  */
    WHITESPACE_P = 761,            /* WHITESPACE_P  */
    WINDOW = 762,                  /* WINDOW  */
    WITH = 763,                    /* WITH  */
    WITHIN = 764,                  /* WITHIN  */
    WITHOUT = 765,                 /* WITHOUT  */
    WORK = 766,                    /* WORK  */
    WRAPPER = 767,                 /* WRAPPER  */
    WRITE_P = 768,                 /* WRITE_P  */
    XML_P = 769,                   /* XML_P  */
    XMLATTRIBUTES = 770,           /* XMLATTRIBUTES  */
    XMLCONCAT = 771,               /* XMLCONCAT  */
    XMLELEMENT = 772,              /* XMLELEMENT  */
    XMLEXISTS = 773,               /* XMLEXISTS  */
    XMLFOREST = 774,               /* XMLFOREST  */
    XMLNAMESPACES = 775,           /* XMLNAMESPACES  */
    XMLPARSE = 776,                /* XMLPARSE  */
    XMLPI = 777,                   /* XMLPI  */
    XMLROOT = 778,                 /* XMLROOT  */
    XMLSERIALIZE = 779,            /* XMLSERIALIZE  */
    XMLTABLE = 780,                /* XMLTABLE  */
    YEAR_P = 781,                  /* YEAR_P  */
    YEARS_P = 782,                 /* YEARS_P  */
    YES_P = 783,                   /* YES_P  */
    ZONE = 784,                    /* ZONE  */
    NOT_LA = 785,                  /* NOT_LA  */
    NULLS_LA = 786,                /* NULLS_LA  */
    WITH_LA = 787,                 /* WITH_LA  */
    POSTFIXOP = 788,               /* POSTFIXOP  */
    UMINUS = 789                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 14 "third_party/libpg_query/grammar/grammar.y"

	core_YYSTYPE		core_yystype;
	/* these fields must match core_YYSTYPE: */
	int					ival;
	char				*str;
	const char			*keyword;
	const char          *conststr;

	char				chr;
	bool				boolean;
	PGJoinType			jtype;
	PGDropBehavior		dbehavior;
	PGOnCommitAction		oncommit;
	PGOnCreateConflict		oncreateconflict;
	PGList				*list;
	PGNode				*node;
	PGValue				*value;
	PGObjectType			objtype;
	PGTypeName			*typnam;
	PGObjectWithArgs		*objwithargs;
	PGDefElem				*defelt;
	PGSortBy				*sortby;
	PGWindowDef			*windef;
	PGJoinExpr			*jexpr;
	PGIndexElem			*ielem;
	PGAlias				*alias;
	PGRangeVar			*range;
	PGIntoClause			*into;
	PGCTEMaterialize			ctematerialize;
	PGWithClause			*with;
	PGInferClause			*infer;
	PGOnConflictClause	*onconflict;
	PGOnConflictActionAlias onconflictshorthand;
	PGAIndices			*aind;
	PGResTarget			*target;
	PGInsertStmt			*istmt;
	PGVariableSetStmt		*vsetstmt;
	PGOverridingKind       override;
	PGSortByDir            sortorder;
	PGSortByNulls          nullorder;
	PGIgnoreNulls          ignorenulls;
	PGConstrType           constr;
	PGLockClauseStrength lockstrength;
	PGLockWaitPolicy lockwaitpolicy;
	PGSubLinkType subquerytype;
	PGViewCheckOption viewcheckoption;
	PGInsertColumnOrder bynameorposition;
	PGLoadInstallType loadinstalltype;
	PGTransactionStmtType transactiontype;
	PGMergeAction mergeaction;

#line 650 "third_party/libpg_query/grammar/grammar_out.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif




int base_yyparse (core_yyscan_t yyscanner);


#endif /* !YY_BASE_YY_THIRD_PARTY_LIBPG_QUERY_GRAMMAR_GRAMMAR_OUT_HPP_INCLUDED  */
