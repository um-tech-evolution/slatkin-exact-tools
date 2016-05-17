# Julia bindings for slatkin-exact-tools
# For now, be sure to run `make shared` before using.

immutable SlatkinResult
  probability::Float64
  theta_estimate::Float64
end

function ewens_montecarlo(num_reps::Int, obs_list::Vector{Int32})
  obs = Int32[0; obs_list; 0]

  ccall((:slatkin_mc, "./slatkin.so"), SlatkinResult,
      (Int, Ptr{Int32}), Int32(num_reps), obs)
end

