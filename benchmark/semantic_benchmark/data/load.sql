COPY cast_info FROM 'benchmark/cast_info.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY info_type FROM 'benchmark/info_type.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY movie FROM 'benchmark/movie.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY movie_info FROM 'benchmark/movie_info.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY "name" FROM 'benchmark/name.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY review FROM 'benchmark/review.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
COPY role_type FROM 'benchmark/role_type.csv' (FORMAT 'csv', quote '"', header 1, ESCAPE '"', DELIMITER ',');
