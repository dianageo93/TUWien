function parse_results_cilk

cilk_csv = 'results_cilk.csv';

data = csvread (cilk_csv, 1, 0);

range100 = data (data (:, 2) == 100, :);
range200 = data (data (:, 2) == 200, :);
range500 = data (data (:, 2) == 500, :);
range1000 = data (data (:, 2) == 500, :);
range5000 = data (data (:, 2) == 500, :);

% Range 100
figure(1);
num_elems = log10 (range100(:, 1));
num_cores = range100(:, 3);
time_par = range100(:, end);
time_seq = range100(:, 4);

F = TriScatteredInterp (num_cores, num_elems, time_par);
F2 = TriScatteredInterp (num_cores, num_elems, time_seq);

ti = min (num_elems) : 0.1 : max (num_elems);
ti2 = min (num_cores) : 0.1 : max (num_cores);

[qx qy] = meshgrid (ti2, ti);

qz = F (qx, qy);
qz2 = F2 (qx, qy);

subplot (1, 2, 1);
mesh (qx, qy, qz);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting parallel, Range = 100');

subplot (1, 2, 2);
mesh (qx, qy, qz2);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting sequential, Range = 100');


% Range 200
figure(2);
num_elems = log10 (range200(:, 1));
num_cores = range200(:, 3);
time_par = range200(:, end);
time_seq = range200(:, 4);

F = TriScatteredInterp (num_cores, num_elems, time_par);
F2 = TriScatteredInterp (num_cores, num_elems, time_seq);

ti = min (num_elems) : 0.1 : max (num_elems);
ti2 = min (num_cores) : 0.1 : max (num_cores);

[qx qy] = meshgrid (ti2, ti);

qz = F (qx, qy);
qz2 = F2 (qx, qy);

subplot (1, 2, 1);
mesh (qx, qy, qz);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting parallel, Range = 200');

subplot (1, 2, 2);
mesh (qx, qy, qz2);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting sequential, Range = 200');


% Range 500
figure(3);
num_elems = log10 (range500(:, 1));
num_cores = range500(:, 3);
time_par = range500(:, end);
time_seq = range500(:, 4);

F = TriScatteredInterp (num_cores, num_elems, time_par);
F2 = TriScatteredInterp (num_cores, num_elems, time_seq);

ti = min (num_elems) : 0.1 : max (num_elems);
ti2 = min (num_cores) : 0.1 : max (num_cores);

[qx qy] = meshgrid (ti2, ti);

qz = F (qx, qy);
qz2 = F2 (qx, qy);

subplot (1, 2, 1);
mesh (qx, qy, qz);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting parallel, Range = 500');

subplot (1, 2, 2);
mesh (qx, qy, qz2);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting sequential, Range = 500');



% Range 1000
figure(4);
num_elems = log10 (range1000(:, 1));
num_cores = range1000(:, 3);
time_par = range1000(:, end);
time_seq = range1000(:, 4);

F = TriScatteredInterp (num_cores, num_elems, time_par);
F2 = TriScatteredInterp (num_cores, num_elems, time_seq);

ti = min (num_elems) : 0.1 : max (num_elems);
ti2 = min (num_cores) : 0.1 : max (num_cores);

[qx qy] = meshgrid (ti2, ti);

qz = F (qx, qy);
qz2 = F2 (qx, qy);

subplot (1, 2, 1);
mesh (qx, qy, qz);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting parallel, Range = 1000');

subplot (1, 2, 2);
mesh (qx, qy, qz2);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting sequential, Range = 1000');


% Range 5000
figure(5);
num_elems = log10 (range5000(:, 1));
num_cores = range5000(:, 3);
time_par = range5000(:, end);
time_seq = range5000(:, 4);

F = TriScatteredInterp (num_cores, num_elems, time_par);
F2 = TriScatteredInterp (num_cores, num_elems, time_seq);

ti = min (num_elems) : 0.1 : max (num_elems);
ti2 = min (num_cores) : 0.1 : max (num_cores);

[qx qy] = meshgrid (ti2, ti);

qz = F (qx, qy);
qz2 = F2 (qx, qy);

subplot (1, 2, 1);
mesh (qx, qy, qz);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting parallel, Range = 5000');

subplot (1, 2, 2);
mesh (qx, qy, qz2);
xlabel ('Number of cores');
ylabel ('Number of elements');
zlabel ('Time');
title ('Sorting sequential, Range = 5000');

end