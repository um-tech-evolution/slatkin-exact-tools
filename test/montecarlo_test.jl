cd("../")
include("$(pwd())/slatkin.jl")

function approx(value, target, message)
  @assert value < target + 0.01 message
  @assert value > target - 0.01 message
end

counts1 = Int32[8, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1]
result1 = ewens_montecarlo(Int32(100000), counts1)

approx(result1.probability, 0.72839, "result1 - probability")
approx(result1.theta_estimate, 6.94400606155, "result1 - theta")

counts2 = Int32[91, 56, 27, 9, 7, 3, 1]
result2 = ewens_montecarlo(Int32(100000), counts2)

approx(result2.probability, 0.31508, "result2 - probability")
approx(result2.theta_estimate, 1.28152208328, "result2 - theta")

