# Julia bindings for slatkin-exact-tools

immutable SlatkinResult
  probability::Cdouble
  theta_estimate::Cdouble
end

function montecarlo(num_reps::Int, obs_list::Vector{Int},
    numalleles::Int, parallel::Int)
  obs = [0; obs_list; 0]

  ccall((:slatkin_mc, "./slatkin.so"), SlatkinResult,
      (Int, Ptr{Int}), num_reps, obs)
end

function ewens_montecarlo(num_reps::Int, obs_list::Vector{Int})
  parallel = 0
  padded_numalleles = length(obs_list) + 2

  montecarlo(num_reps, obs_list, padded_numalleles, parallel)
end

println(ewens_montecarlo(100000, [8, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1]))

