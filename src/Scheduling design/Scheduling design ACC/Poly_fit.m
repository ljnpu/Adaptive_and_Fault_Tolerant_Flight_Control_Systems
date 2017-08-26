function [fitresult, gof] = Poly_fit(M_eval_sche, H_eval_sche, tau_AoA_extrap)
%CREATEFIT2(M_EVAL_SCHE,H_EVAL_SCHE,TAU_AOA_EXTRAP)
%  Create a fit.
%
%  Data for 'untitled fit 1' fit:
%      X Input : M_eval_sche
%      Y Input : H_eval_sche
%      Z Output: tau_AoA_extrap
%  Output:
%      fitresult : a fit object representing the fit.
%      gof : structure with goodness-of fit info.
%
%  See also FIT, CFIT, SFIT.

%  Auto-generated by MATLAB on 24-Jul-2014 18:05:07


%% Fit: 'untitled fit 1'.
[xData, yData, zData] = prepareSurfaceData( M_eval_sche, H_eval_sche, tau_AoA_extrap );

% Set up fittype and options.
ft = fittype( 'poly44' );
opts = fitoptions( ft );
opts.Lower = [-Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf -Inf];
opts.Upper = [Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf Inf];

% Fit model to data.
[fitresult, gof] = fit( [xData, yData], zData, ft, opts );

% Plot fit with data.
figure( 'Name', 'untitled fit 1' );
h = plot( fitresult, [xData, yData], zData );
legend( h, 'untitled fit 1', 'tau_AoA_extrap vs. M_eval_sche, H_eval_sche', 'Location', 'NorthEast' );
% Label axes
xlabel( 'M_eval_sche' );
ylabel( 'H_eval_sche' );
zlabel( 'tau_AoA_extrap' );
grid on
view( -2.5, 44 );


