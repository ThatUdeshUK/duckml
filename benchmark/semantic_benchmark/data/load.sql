COPY cast_info FROM 'benchmark/semantic_benchmark/data/cast_info.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY info_type FROM 'benchmark/semantic_benchmark/data/info_type.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY movie FROM 'benchmark/semantic_benchmark/data/movie.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY movie_info FROM 'benchmark/semantic_benchmark/data/movie_info.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY movie_sample FROM 'benchmark/semantic_benchmark/data/movie_sample.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY "name" FROM 'benchmark/semantic_benchmark/data/name.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY review FROM 'benchmark/semantic_benchmark/data/review.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
COPY role_type FROM 'benchmark/semantic_benchmark/data/role_type.csv' (FORMAT 'csv', quote '"', delimiter ',', header 1);
