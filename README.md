# Description 

Consider an expressway described as a sequence of service stations. Each service station is located at a distance from the beginning of the expressway expressed in kilometers as a positive integer or zero. There are no two service stations with the same distance: each service station is uniquely identified by its distance from the beginning of the expressway.

### Stations

Each service station is equipped with a fleet of electric rental vehicles. Each vehicle is characterized by its range, expressed in kilometers, as a positive integer. The fleet of a single station includes at most 512 vehicles. When renting a car from station s, it is possible to reach all those whose distance from s is less than or equal to the vehicle's range.

### Journey

A journey is identified by a sequence of service stations where the driver makes stops. It begins at a service station and ends at another, passing through zero or more intermediate stations. It is assumed that the driver cannot backtrack during the journey and rents a new car every time they make a stop at a service station. Therefore, given two consecutive stops s and t, t must always be further from the starting point than s, and t must be reachable using one of the vehicles available at s.

### Objective

The objective of the project is as follows: given a pair of stations, plan the route with the fewest number of stops between them. In case there are multiple routes with the same minimum number of stops (i.e., tied), the route that prefers stops with shorter distances from the beginning of the expressway should be chosen. In other words, consider the set of n tied routes P = {p1, p2, . . . pn} where each route is a tuple of m elements pi = ⟨pi,1, pi,2, . . . pi,m⟩ representing the distance from the beginning of the expressway of each stop in order of traversal. Choose the unique route pi such that there is no other route pj with the same last k stops preceded by a stop with a shorter distance, i.e., ∄j, k : ⟨pi,m−k+1, . . . pi,m⟩ = ⟨pj,m−k+1, . . . pj,m⟩ ∧ pj,m−k < pi,m−k.
