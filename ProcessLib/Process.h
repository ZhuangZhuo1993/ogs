/**
 * \copyright
 * Copyright (c) 2012-2016, OpenGeoSys Community (http://www.opengeosys.org)
 *            Distributed under a Modified BSD License.
 *              See accompanying file LICENSE.txt or
 *              http://www.opengeosys.org/project/license
 *
 */

#ifndef PROCESS_LIB_PROCESS_H_
#define PROCESS_LIB_PROCESS_H_

#include <string>

#include "AssemblerLib/LocalToGlobalIndexMap.h"
#include "BaseLib/ConfigTree.h"

#ifdef USE_PETSC
#include "MeshLib/NodePartitionedMesh.h"
#include "MathLib/LinAlg/PETSc/PETScMatrixOption.h"
#endif

namespace MeshLib
{
class Mesh;
}

namespace ProcessLib
{
template <typename GlobalSetup>
class Process
{
public:
	Process(MeshLib::Mesh& mesh) : _mesh(mesh) {}
	virtual ~Process() = default;

	virtual void initialize() = 0;
	virtual bool assemble(const double delta_t) = 0;

	/// Postprocessing after solve().
	/// The file_name is indicating the name of possible output file.
	virtual void post(std::string const& file_name) = 0;
	virtual void postTimestep(std::string const& file_name,
	                          const unsigned timestep) = 0;

	bool solve(const double delta_t)
	{
		bool const result = assemble(delta_t);

		_linear_solver->solve(*_rhs, *_x);
		return result;
	}

protected:
	/// Set linear solver options; called by the derived process which is
	/// parsing the configuration.
	void setLinearSolverOptions(const BaseLib::ConfigTree& config)
	{
		_linear_solver_options.reset(new BaseLib::ConfigTree(config));
	}

	/// Creates global matrix, rhs and solution vectors, and the linear solver.
	void createLinearSolver(
	    AssemblerLib::LocalToGlobalIndexMap const& local_to_global_index_map,
	    std::string const solver_name)
	{
		DBUG("Allocate global matrix, vectors, and linear solver.");
#ifdef USE_PETSC
		MathLib::PETScMatrixOption mat_opt;
		const MeshLib::NodePartitionedMesh& pmesh =
		    static_cast<const MeshLib::NodePartitionedMesh&>(_mesh);
		mat_opt.d_nz = pmesh.getMaximumNConnectedNodesToNode();
		mat_opt.o_nz = mat_opt.d_nz;
		const std::size_t num_unknowns =
		    local_to_global_index_map.dofSizeGlobal();
		_A.reset(_global_setup.createMatrix(num_unknowns, mat_opt));
#else
		const std::size_t num_unknowns = local_to_global_index_map.dofSize();
		_A.reset(_global_setup.createMatrix(num_unknowns));
#endif
		_x.reset(_global_setup.createVector(num_unknowns));
		_rhs.reset(_global_setup.createVector(num_unknowns));
		_linear_solver.reset(new typename GlobalSetup::LinearSolver(
		    *_A, solver_name, _linear_solver_options.get()));
	}


protected:
	MeshLib::Mesh& _mesh;

	GlobalSetup _global_setup;

	std::unique_ptr<BaseLib::ConfigTree> _linear_solver_options;
	std::unique_ptr<typename GlobalSetup::LinearSolver> _linear_solver;

	std::unique_ptr<typename GlobalSetup::MatrixType> _A;
	std::unique_ptr<typename GlobalSetup::VectorType> _rhs;
	std::unique_ptr<typename GlobalSetup::VectorType> _x;
};

}  // namespace ProcessLib

#endif  // PROCESS_LIB_PROCESS_H_
