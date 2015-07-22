﻿using System;
using System.Linq;
using System.Runtime.Versioning;
using Microsoft.Framework.Runtime;
using Microsoft.Framework.Runtime.Compilation;

namespace Microsoft.Framework.PackageManager
{
    public class ProjectBuilder
    {
        private readonly ILibraryExporter _libraryExporter;

        public ProjectBuilder(ILibraryExporter libraryExporter)
        {
            _libraryExporter = libraryExporter;
        }

        public DiagnosticResult Build(string name, string outputPath)
        {
            var export = _libraryExporter.GetLibraryExport(name);

            if (export == null)
            {
                return null;
            }

            foreach (var projectReference in export.MetadataReferences.OfType<IMetadataProjectReference>())
            {
                if (string.Equals(projectReference.Name, name, StringComparison.OrdinalIgnoreCase))
                {
                    return projectReference.EmitAssembly(outputPath);
                }
            }

            return null;
        }
    }
}