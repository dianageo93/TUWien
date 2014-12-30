function parse_results

%parse_results_pthreads;
parse_results_cilk;

end


%subplot (1, 2, 1);

%[x y z] = gridfit (num_cores, time_par, num_elems, 20, 30);
%surfc (x, y, z);

%xlabel ('Number elements');
%ylabel ('Number cores');
%zlabel ('Time parallel');

%title ('Range = 100 parallel time');

%subplot (1, 2, 2);

%[x y z] = gridfit (num_cores, time_seq, num_elems, 20, 30);
%surfc (x, y, z);

%xlabel ('Number elements');
%ylabel ('Number cores');
%zlabel ('Time parallel');

%title ('Range = 100 sequential time');




% Range 5000
%figure(2);
%num_elems = log10(range5000 (:, 1));
%num_cores = range5000 (:, 3);
%time_par = range5000 (:, end);
%time_seq = range5000 (:, 4);

%subplot (1, 2, 1);

%[x y z] = gridfit (num_cores, time_par, num_elems, 20, 30);
%surfc (x, y, z);

%xlabel ('Number elements');
%ylabel ('Number cores');
%zlabel ('Time parallel');

%title ('Range = 100 parallel time');

%subplot (1, 2, 2);

%[x y z] = gridfit (num_cores, time_seq, num_elems, 20, 30);
%surfc (x, y, z);

%xlabel ('Number elements');
%ylabel ('Number cores');
%zlabel ('Time parallel');

%title ('Range = 100 sequential time');
